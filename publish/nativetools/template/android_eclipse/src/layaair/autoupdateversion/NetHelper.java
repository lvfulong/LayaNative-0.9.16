package layaair.autoupdateversion;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.CoreProtocolPNames;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import android.util.Log;

public class NetHelper {

    public static String httpStringGet(String url) throws Exception {
        return httpStringGet(url, "utf-8");
    }

    public static String httpStringGet(String url, String enc) throws Exception {
        // This method for HttpConnection
        String page;
        BufferedReader bufferedReader = null;
        try {
            HttpClient client = new DefaultHttpClient();
            client.getParams().setParameter(CoreProtocolPNames.USER_AGENT,
                    "android");
            
             HttpParams httpParams = client.getParams();
             HttpConnectionParams.setConnectionTimeout(httpParams, 3000);
             HttpConnectionParams.setSoTimeout(httpParams, 5000);
             
            HttpGet request = new HttpGet();
            request.setHeader("Content-Type", "text/plain; charset=utf-8");
            request.setURI(new URI(url));
            HttpResponse response = client.execute(request);
            bufferedReader = new BufferedReader(new InputStreamReader(response
                    .getEntity().getContent(), enc));

            StringBuilder stringBuffer = new StringBuilder("");
            String line;

            String NL = System.getProperty("line.separator");
            while ((line = bufferedReader.readLine()) != null) {
                stringBuffer.append(line).append(NL);
            }
            bufferedReader.close();
            page = stringBuffer.toString();
            Log.i("page", page);
            System.out.println(page + "page");
            return page;
        } finally {
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    Log.d("BBB", e.toString());
                }
            }
        }
    }
}
