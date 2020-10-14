#include <jni.h>
#include <string>
#include <oboe/Oboe.h>
#include "logging_macros.h"

// 这部分变量是采样相关的 , 与 Oboe 操作无关
// 声道个数 , 2 代表立体声
static int constexpr kChannelCount = 2;
static int constexpr kSampleRate = 48000;
// Wave params, these could be instance variables in order to modify at runtime
static float constexpr kAmplitude = 0.5f;
// 频率
static float constexpr kFrequency = 440;
// PI 圆周率
static float constexpr kPI = M_PI;
// 2 PI 两倍圆周率
static float constexpr kTwoPi = kPI * 2;
// 每次累加的采样值
static double constexpr mPhaseIncrement = kFrequency * kTwoPi / (double) kSampleRate;
// 追踪当前波形位置
float mPhase = 0.0;

// Oboe 音频流回调类
class MyCallback : public oboe::AudioStreamCallback {
public:
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {

        // 需要生成 AudioFormat::Float 类型数据 , 该缓冲区类型也是该类型
        // 生产者需要检查该格式
        // oboe::AudioStream *audioStream 已经转换为适当的类型
        // 获取音频数据缓冲区
        auto *floatData = static_cast<float *>(audioData);

        // 生成正弦波数据
        for (int i = 0; i < numFrames; ++i) {
            float sampleValue = kAmplitude * sinf(mPhase);
            for (int j = 0; j < kChannelCount; j++) {
                floatData[i * kChannelCount + j] = sampleValue;
            }
            mPhase += mPhaseIncrement;
            if (mPhase >= kTwoPi) mPhase -= kTwoPi;
        }

        LOGI("回调 onAudioReady");

        return oboe::DataCallbackResult::Continue;
    }
};

// 创建 MyCallback 对象
MyCallback myCallback = MyCallback();
// 声明 Oboe 音频流
oboe::ManagedStream managedStream = oboe::ManagedStream();


extern "C" JNIEXPORT jstring JNICALL
Java_kim_hsl_oboedemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    // 1. 音频流构建器
    oboe::AudioStreamBuilder builder = oboe::AudioStreamBuilder();
    // 设置音频流方向
    builder.setDirection(oboe::Direction::Output);
    // 设置性能优先级
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    // 设置共享模式 , 独占
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    // 设置音频采样格式
    builder.setFormat(oboe::AudioFormat::Float);
    // 设置声道数 , 单声道/立体声
    builder.setChannelCount(oboe::ChannelCount::Stereo);
    // 设置采样率
    builder.setSampleRate(48000);
    // 设置回调对象 , 注意要设置 AudioStreamCallback * 指针类型
    builder.setCallback(&myCallback);


    // 2. 通过 AudioStreamBuilder 打开 Oboe 音频流
    oboe::Result result = builder.openManagedStream(managedStream);
    LOGI("openManagedStream result : %s", oboe::convertToText(result));

    // 3. 开始播放
    result = managedStream->requestStart();
    LOGI("requestStart result : %s", oboe::convertToText(result));

    // 返回数据到
    std::string hello = "Oboe Test " + std::to_string(static_cast<int>(oboe::PerformanceMode::LowLatency)) + " Result : " + oboe::convertToText(result);
    return env->NewStringUTF(hello.c_str());
}
