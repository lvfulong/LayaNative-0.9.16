package layaair.game.Notifycation;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

public class LayaLocalNotify 
{
	private static NotificationManager 		m_pNotificationManager;
	private static Intent 					m_pIntent;

	public static void sendNotification( Context p_pContext,int p_nID,String p_sTickertext,String p_sTitle,String p_sContent )
	{
		m_pNotificationManager =  (NotificationManager)p_pContext.getSystemService( Context.NOTIFICATION_SERVICE );
		int nDrawableId = p_pContext.getResources().getIdentifier("ic_launcher","drawable", p_pContext.getPackageName());
		Notification m_pNotification = new Notification(nDrawableId, p_sTickertext, System.currentTimeMillis());
		m_pNotification.defaults=Notification.DEFAULT_ALL;
		m_pIntent = new Intent( p_pContext, p_pContext.getClass());
		//m_pIntent = new Intent( p_pContext, MainActivity.class );
		PendingIntent m_pPendingIntent = PendingIntent.getActivity(p_pContext, 0, m_pIntent, 0);
		m_pNotification.setLatestEventInfo(p_pContext, p_sTitle, p_sContent, m_pPendingIntent);
		m_pNotificationManager.notify( p_nID, m_pNotification);
		
	}
	public static void removeNotfication( Context p_pContext,int p_nID )
	{
		//Log.i("0","========removeNotfication");
		m_pNotificationManager =  (NotificationManager)p_pContext.getSystemService(Context.NOTIFICATION_SERVICE);
		m_pNotificationManager.cancel( p_nID );
	}
	public static void removeAllNotification( Context p_pContext )
	{
		m_pNotificationManager =  (NotificationManager)p_pContext.getSystemService(Context.NOTIFICATION_SERVICE);
		m_pNotificationManager.cancelAll();
	}
}