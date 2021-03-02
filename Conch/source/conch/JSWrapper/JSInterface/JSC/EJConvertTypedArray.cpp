#include "EJConvertTypedArray.h"
#include "EJConvert.h"

#if defined __ARM_NEON__
#include <arm_neon.h>
#endif
namespace laya
{
// These functions deal with getting and setting a JavaScript Typed Array's data.
// The process in which this is done is extremely backwards, prohibitively
// memory inefficient and painfully slow.

// JavaScriptCore still doesn't have an API to deal with Typed Arrays directly
// and nobody seems to care. See this WebKit bug for the details:
// https://bugs.webkit.org/show_bug.cgi?id=120112

// The naive method of getting the array's data would be to read the "length"
// property and call JSObjectGetPropertyAtIndex() for each element. This function
// returns a JSValue and since the array's data is just a raw memory pointer
// internally, this JSValue has be constructed first by JSC. Afterwards this
// JSValue has to be converted back into an int and stored in a native buffer.
// This ordeal takes about 1200ms per megabyte on an iPhone5S.

// So, instead of extracting each byte on its own, we create Int32 view on the
// Array, so that we can extract 4 bytes at a time. Now, the fastest method to
// get to the values is to call a function with .apply(null, theInt32Array).
// This causes the typed array to be converted into plain JSValues - we can then
// iterate all function arguments and convert each JSValue into a Int32.

// There's one big caveat, though: a function can only be called with 64k
// arguments at once, or it will blow the stack. So we have to divide the Int32
// Array into smaller subarrays and convert the data in chunks.

// Setting the Array's data works about the same. We can create a plain JS
// Array in reasonable time by treating the data as 32bit ints. This plain
// Array can then be used to call .set() on an Int32 View of the Typed Array.

// To be able to quickly determine the type of a typed array, we install an
// enum with the type id ("__ejTypedArrayType") onto each array constructor's
// prototype.

// This all brings the time down to about 7ms per megabyte for reading and about
// 20ms per megabyte for writing. Even though this 7ms/MB number may not sound
// all too bad, keep in mind that it's still 7ms slower than what we would have
// with a better API.





// Array Types to constructor names. We need to obtain the Constructors from
// the JSGlobalObject when creating Typed Arrays and when attaching our methods
// to them.

const static char *ConstructorNames[] = {
	[_kJSTypedArrayTypeNone] = NULL,
	[_kJSTypedArrayTypeInt8Array] = "Int8Array",
	[_kJSTypedArrayTypeInt16Array] = "Int16Array",
	[_kJSTypedArrayTypeInt32Array] = "Int32Array",
	[_kJSTypedArrayTypeUint8Array] = "Uint8Array",
	[_kJSTypedArrayTypeUint8ClampedArray] = "Uint8ClampedArray",
	[_kJSTypedArrayTypeUint16Array] = "Uint16Array",
	[_kJSTypedArrayTypeUint32Array] = "Uint32Array",
	[_kJSTypedArrayTypeFloat32Array] = "Float32Array",
	[_kJSTypedArrayTypeFloat64Array] = "Float64Array",
	[_kJSTypedArrayTypeArrayBuffer] = "ArrayBuffer"
};


// Data from Typed Arrays smaller than CopyInChunksThreshold will be extracted
// one byte at a time.

const static int CopyInChunksThreshold = 32;


// For large arrays, copy the data in Chunks of 16k elements, so that we don't
// blow the stack.

const static int CopyChunkSize = 0x4000;



// Hobo version to get an Int32 from a JSValueRef. This is even faster than
// JSValueToNumberFast() because it blindly assumes the value is an Int32.
// A number stored as double will produce a bogus Int value. JSC stores all
// values that are outside of the Int32 range as double.

// This function is only fast on 64bit systems. On 32bit systems we can't
// easily access the integer value.
// See JavaScriptCore/runtime/JSCJSValue.h for an explanation of the NaN-
// boxing used in JSC.

// Use with extreme caution!

static inline int32_t GetInt32(JSContextRef ctx, JSValueRef v) {
	#if __LP64__
		return (int32_t)(0x00000000ffffffffll & (uint64_t)v);
	#else
		return JSValueToNumber(ctx, v, NULL);
	#endif
}


// Hobo version to create an Int32 fast. Falls back to JSValueMakeNumber()
// on 32bit systems.

static inline JSValueRef MakeInt32(JSContextRef ctx, int32_t number) {
	#if __LP64__
		return (JSValueRef)(0xffff000000000000ll | (uint64_t)number);
	#else
		return JSValueMakeNumber(ctx, number);
	#endif
}


// Shorthand to get a property by name

static JSValueRef GetPropertyNamed(JSContextRef ctx, JSObjectRef object, const char *name) {
	JSStringRef jsPropertyName = JSStringCreateWithUTF8CString(name);
	JSValueRef value = JSObjectGetProperty(ctx, object, jsPropertyName, NULL);
	JSStringRelease(jsPropertyName);
	return value;
}


// Shorthand to get the Constructor for the given JSTypedArrayType

static JSObjectRef GetConstructor(JSContextRef ctx, _JSTypedArrayType type) {
	if( type <= _kJSTypedArrayTypeNone || type > _kJSTypedArrayTypeArrayBuffer ) {
		return NULL;
	}
	
	const char *constructorName = ConstructorNames[type];
	JSObjectRef global = JSContextGetGlobalObject(ctx);
	return (JSObjectRef)GetPropertyNamed(ctx, global, constructorName);
}


// Create a typed array view from another typed array or arraybuffer

static JSObjectRef GetView(JSContextRef ctx, JSObjectRef object, _JSTypedArrayType type, size_t count) {
	_JSTypedArrayType currentType = JSObjectGetTypedArrayType(ctx, object);
	if( currentType == _kJSTypedArrayTypeNone ) {
		return NULL;
	}
	else if( currentType == type ) {
		return object;
	}
	
	JSValueRef args[3];
	if( currentType == _kJSTypedArrayTypeArrayBuffer ) {
		args[0] = object;
		args[1] = MakeInt32(ctx, 0);
		args[2] = MakeInt32(ctx, (int)count);
	}
	else {
		args[0] = GetPropertyNamed(ctx, object, "buffer");
		args[1] = GetPropertyNamed(ctx, object, "byteOffset");
		args[2] = MakeInt32(ctx, (int)count);
	}
	JSObjectRef constructor = GetConstructor(ctx, type);
	return JSObjectCallAsConstructor(ctx, constructor, 3, args, NULL);
}


// The callback called from JSObjectGetTypedArrayData with chunks of data. This writes
// into the state's data.

typedef struct {
	int32_t *currentDataPtr;
	JSObjectRef jsGetCallback;
	JSObjectRef jsGetCallbackApply;
} AppendDataCallbackState;

static JSValueRef AppendDataCallback(
	JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
	size_t argc, const JSValueRef argv[], JSValueRef* exception
) {
	AppendDataCallbackState *state = (AppendDataCallbackState*)JSObjectGetPrivate(thisObject);
	int32_t *dst = state->currentDataPtr;
	int remainderStart = 0;
	
	// On 64bit systems where ARM_NEON instructions are available we can use
	// some SIMD intrinsics to extract the lower 32bit of each JSValueRef.
	// Hopefully the JSValueRef encodes an Int32 so the lower 32bit corresponds
	// to that Int32 exactly.
	
	#if __LP64__ && defined __ARM_NEON__
		// Iterate over the arguments in packs of 4.
		// Load arguments as 2 lanes of 4 int32 values and store the first
		// lane (the lower 32bit) into the dst buffer.
	
		int argPacks4 = (int)argc/4;
		int32_t *src = (int32_t*)argv;
	
		for(int i = 0; i < argPacks4; i++ ) {
			const int32x4x2_t lanes32 = vld2q_s32(src);
			vst1q_s32(dst, lanes32.val[0]);
			src += 8;
			dst += 4;
		}
		remainderStart = argPacks4 * 4;
	#endif
	
	for( int i = remainderStart; i < argc; i++ ) {
		*(dst++) = GetInt32(ctx, argv[i]);
	}
	
	state->currentDataPtr += argc;
	return MakeInt32(ctx, (int)argc);
}


// To store the AppendDataCallbackState and access it _per context_, we create
// a plain JSObject where we can store a pointer to the state in the private
// data.

static void FinalizeAppendDataCallbackState(JSObjectRef object) {
	AppendDataCallbackState *state = (AppendDataCallbackState*)JSObjectGetPrivate(object);
	free(state);
}

static JSObjectRef CreateAppendDataCallbackState(JSContextRef ctx) {
	// Create a JS function that calls the AppendDataCallback
	JSObjectRef jsAppendDataCallback = JSObjectMakeFunctionWithCallback(ctx, NULL, AppendDataCallback);
	JSValueProtect(ctx, jsAppendDataCallback);
	
	// Allocate and create the state and attach the AppendDataCallback
	// function and its .apply property.
	AppendDataCallbackState *state = (AppendDataCallbackState*)malloc(sizeof(AppendDataCallbackState));
	state->currentDataPtr = NULL;
	state->jsGetCallback = jsAppendDataCallback;
	state->jsGetCallbackApply = (JSObjectRef)GetPropertyNamed(ctx, jsAppendDataCallback, "apply");
	
	
	// Create the empty js object that holds the state in its Private data
	JSClassDefinition internalStateClassDef = kJSClassDefinitionEmpty;
	internalStateClassDef.finalize = FinalizeAppendDataCallbackState;
	
	JSClassRef internalStateClass = JSClassCreate(&internalStateClassDef);
	JSObjectRef internalStateObject = JSObjectMake(ctx, internalStateClass, state);
	JSClassRelease(internalStateClass);
	return internalStateObject;
}




void JSContextPrepareTypedArrayAPI(JSContextRef ctx) {
	// The __ejTypedArrayType property is read only, not enumerable
	JSPropertyAttributes attributes =
		kJSPropertyAttributeReadOnly |
		kJSPropertyAttributeDontEnum |
		kJSPropertyAttributeDontDelete;
	
	// Install the __ejTypedArrayType property on each Typed Array prototype
	JSStringRef jsTypeName = JSStringCreateWithUTF8CString("__ejTypedArrayType");
	
	for( int type = _kJSTypedArrayTypeInt8Array; type <= _kJSTypedArrayTypeArrayBuffer; type++ ) {
		JSObjectRef jsConstructor = GetConstructor(ctx, (_JSTypedArrayType)type);
		JSObjectRef jsPrototype = (JSObjectRef)GetPropertyNamed(ctx, jsConstructor, "prototype");
		
		JSValueRef jsType = MakeInt32(ctx, type);
		JSObjectSetProperty(ctx, jsPrototype, jsTypeName, jsType, attributes, NULL);
	}
	
	JSStringRelease(jsTypeName);
	
	
	// Create the state object for the AppendData Callback and attach it to the global
	// object
	JSObjectRef jsCallbackStateObject = CreateAppendDataCallbackState(ctx);

	JSStringRef jsInternalStateName = JSStringCreateWithUTF8CString("__ejTypedArrayState");
	JSObjectRef global = JSContextGetGlobalObject(ctx);
	JSObjectSetProperty(ctx, global, jsInternalStateName, jsCallbackStateObject, attributes, NULL);
	JSStringRelease(jsInternalStateName);
}


_JSTypedArrayType JSObjectGetTypedArrayType(JSContextRef ctx, JSObjectRef object) {
	JSValueRef jsType = GetPropertyNamed(ctx, object, "__ejTypedArrayType");
	return jsType ? (_JSTypedArrayType)GetInt32(ctx, jsType) : _kJSTypedArrayTypeNone;
}


JSObjectRef JSObjectMakeTypedArrayEJ(JSContextRef ctx, _JSTypedArrayType arrayType, size_t numElements) {
	JSObjectRef jsConstructor = GetConstructor(ctx, arrayType);
	if( !jsConstructor ) {
		return NULL;
	}
	
	JSValueRef jsNumElements = MakeInt32(ctx, (int)numElements);
	return JSObjectCallAsConstructor(ctx, jsConstructor, 1, (JSValueRef[]){jsNumElements}, NULL);
}

bool JSObjectGetTypedArrayData(JSContextRef ctx, JSObjectRef object, char*& data, int& len) {
    if (object == NULL) {
        return false;
    }
    size_t length = GetInt32(ctx, GetPropertyNamed(ctx, object, "byteLength"));
    if( !length ) {
        return false;
    }
    
    size_t int32Count = length / 4;
    size_t uint8Count = length % 4;
    
    // For very small typed arrays it's faster to read all bytes individually
    // instead of doing the callback dance.
    if( length < CopyInChunksThreshold ) {
        int32Count = 0;
        uint8Count = length;
    }
    
    data = new char[length];
    len = length;
    
    // Read data in large chunks of 32bit values
    if( int32Count ) {
        JSObjectRef int32View = GetView(ctx, object, _kJSTypedArrayTypeInt32Array, int32Count);
        if( !int32View ) {
            delete[] data;
            data = NULL;
            len = 0;
            return false;
        }
        
        JSObjectRef jsState = (JSObjectRef)GetPropertyNamed(ctx, JSContextGetGlobalObject(ctx), "__ejTypedArrayState");
        AppendDataCallbackState *state = (AppendDataCallbackState*)JSObjectGetPrivate(jsState);
        state->currentDataPtr = (int32_t*)(&data[0]);
        
        
        // If the whole data is smaller than the chunk size, we only have to call our callback once, otherwise
        // we have to create subarrays and call the callback with these.
        if( int32Count < CopyChunkSize ) {
            JSValueRef getArgs[] = {jsState, int32View};
            JSObjectCallAsFunction(ctx, state->jsGetCallbackApply, state->jsGetCallback, 2, getArgs, NULL);
        }
        else {
            JSObjectRef subarrayFunc = (JSObjectRef)GetPropertyNamed(ctx, int32View, "subarray");
            
            for( int i = 0; i < int32Count; i+= CopyChunkSize) {
                JSValueRef subarrayArgs[] = {MakeInt32(ctx, i), MakeInt32(ctx, i + CopyChunkSize)};
                JSObjectRef jsSubarray = (JSObjectRef)JSObjectCallAsFunction(ctx, subarrayFunc, int32View, 2, subarrayArgs, NULL);
                
                JSValueRef getArgs[] = {jsState, jsSubarray};
                JSObjectCallAsFunction(ctx, state->jsGetCallbackApply, state->jsGetCallback, 2, getArgs, NULL);
            }
        }
    }
    
    // Read remaining bytes directly
    if( uint8Count ) {
        uint8_t *values8 = (uint8_t*)(&data[0]);
        JSObjectRef uint8View = GetView(ctx, object, _kJSTypedArrayTypeUint8Array, length);
        for( int i = 0; i < uint8Count; i++ ) {
            int index = (int)int32Count * 4 + i;
            values8[index] = GetInt32(ctx, JSObjectGetPropertyAtIndex(ctx, uint8View, index, NULL));
        }
    }
    
    return true;
}

bool JSObjectGetTypedArrayDataNoAlloc(JSContextRef ctx, JSObjectRef object, char* data, int len) {
    if (object == NULL) {
        return false;
    }
    size_t length = GetInt32(ctx, GetPropertyNamed(ctx, object, "byteLength"));
    assert(len >= length);
    if( !length ) {
        return false;
    }
    
    size_t int32Count = length / 4;
    size_t uint8Count = length % 4;
    
    // For very small typed arrays it's faster to read all bytes individually
    // instead of doing the callback dance.
    if( length < CopyInChunksThreshold ) {
        int32Count = 0;
        uint8Count = length;
    }
    
    //data = new char[length];
    //len = length;
    
    // Read data in large chunks of 32bit values
    if( int32Count ) {
        JSObjectRef int32View = GetView(ctx, object, _kJSTypedArrayTypeInt32Array, int32Count);
        if( !int32View ) {
            //delete[] data;
            //data = NULL;
            //len = 0;
            return false;
        }
        
        JSObjectRef jsState = (JSObjectRef)GetPropertyNamed(ctx, JSContextGetGlobalObject(ctx), "__ejTypedArrayState");
        AppendDataCallbackState *state = (AppendDataCallbackState*)JSObjectGetPrivate(jsState);
        state->currentDataPtr = (int32_t*)(&data[0]);
        
        
        // If the whole data is smaller than the chunk size, we only have to call our callback once, otherwise
        // we have to create subarrays and call the callback with these.
        if( int32Count < CopyChunkSize ) {
            JSValueRef getArgs[] = {jsState, int32View};
            JSObjectCallAsFunction(ctx, state->jsGetCallbackApply, state->jsGetCallback, 2, getArgs, NULL);
        }
        else {
            JSObjectRef subarrayFunc = (JSObjectRef)GetPropertyNamed(ctx, int32View, "subarray");
            
            for( int i = 0; i < int32Count; i+= CopyChunkSize) {
                JSValueRef subarrayArgs[] = {MakeInt32(ctx, i), MakeInt32(ctx, i + CopyChunkSize)};
                JSObjectRef jsSubarray = (JSObjectRef)JSObjectCallAsFunction(ctx, subarrayFunc, int32View, 2, subarrayArgs, NULL);
                
                JSValueRef getArgs[] = {jsState, jsSubarray};
                JSObjectCallAsFunction(ctx, state->jsGetCallbackApply, state->jsGetCallback, 2, getArgs, NULL);
            }
        }
    }
    
    // Read remaining bytes directly
    if( uint8Count ) {
        uint8_t *values8 = (uint8_t*)(&data[0]);
        JSObjectRef uint8View = GetView(ctx, object, _kJSTypedArrayTypeUint8Array, length);
        for( int i = 0; i < uint8Count; i++ ) {
            int index = (int)int32Count * 4 + i;
            values8[index] = GetInt32(ctx, JSObjectGetPropertyAtIndex(ctx, uint8View, index, NULL));
        }
    }
    
    return true;
}


void JSObjectSetTypedArrayData(JSContextRef ctx, JSObjectRef object, const uint8_t *data, size_t length) {
	size_t int32Count = length / 4;
	size_t uint8Count = length % 4;
	
	if( int32Count ) {
		JSObjectRef int32View = GetView(ctx, object, _kJSTypedArrayTypeInt32Array, int32Count);
		if( !int32View ) {
			return;
		}
		
		// Construct the JSValues and create the plain JSArray
		JSValueRef *jsValues = static_cast<JSValueRef*>(malloc(int32Count  * sizeof(JSValueRef)));
		
		const int32_t *values32 = (int32_t*)data;
		for(int i = 0; i < int32Count; i++) {
			jsValues[i] = MakeInt32(ctx, values32[i]);
		}
		JSObjectRef jsArray = JSObjectMakeArray(ctx, int32Count, jsValues, NULL);
		
		free(jsValues);
		
		// Call the .set() function on the Typed Array
		JSObjectRef setFunction = (JSObjectRef)GetPropertyNamed(ctx, int32View, "set");
		JSObjectCallAsFunction(ctx, setFunction, int32View, 1, (JSValueRef[]){jsArray}, NULL);
	}
	
	// Set remaining bytes directly
	if( uint8Count ) {
		const uint8_t *values8 = data;
		JSObjectRef uint8View = GetView(ctx, object, _kJSTypedArrayTypeUint8Array, length);
		for( int i = 0; i < uint8Count; i++ ) {
			int index = (int)int32Count * 4 + i;
			JSObjectSetPropertyAtIndex(ctx, uint8View, index, MakeInt32(ctx, values8[index]), NULL);
		}
	}
}
}
