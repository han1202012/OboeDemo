package kim.hsl.oboedemo

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity() {

    /**
     * 每次 Resume 第一次忽略
     */
    var isResumeIgnore = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // 创建 Oboe 音频流并发音
        sample_text.text = stringFromJNI()

        val filter = IntentFilter()
        filter.addAction("android.intent.action.HEADSET_PLUG")
        registerReceiver(mHeadsetPlugReceiver, filter)
    }

    override fun onResume() {
        super.onResume()
        isResumeIgnore = true
    }

    override fun onPause() {
        super.onPause()


    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceiver(mHeadsetPlugReceiver)
    }

    /**
     * 广播接收者
     * 监听耳机插拔事件
     */
    val mHeadsetPlugReceiver: BroadcastReceiver = object : BroadcastReceiver(){
        override fun onReceive(context: Context, intent: Intent) {
            if (intent.hasExtra("state")) {

                // resume 第一次忽略耳机插拔事件
                if(isResumeIgnore){
                    isResumeIgnore = false
                    return
                }

                if (intent.getIntExtra("state", 0) == 0) {
                    stringFromJNI()
                    Toast.makeText(context,
                        "耳机拔出", Toast.LENGTH_SHORT).show()


                } else if (intent.getIntExtra("state", 0) == 1) {
                    stringFromJNI()
                    Toast.makeText(context,
                        "耳机插入", Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    /**
     * 重新打开 Oboe 音频流
     */
    external fun stringFromJNI(): String

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}
