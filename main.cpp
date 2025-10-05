#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>
#include "FfmpegVideoReader.h"
#include "VideoFrameProvider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    VideoFrameProvider* frameProvider = new VideoFrameProvider();

    engine.addImageProvider("videoframe", frameProvider);

    FfmpegVideoReader reader;
    if (!reader.open("../../Video/Test_h264.mp4")) {
        qWarning("Không mở được video!");
        return -1;
    }

    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [&]() {
        VideoFrame* vf = reader.readFrame();
        if (vf) {
            QImage img(vf->rgbData.data(), vf->width, vf->height, QImage::Format_RGB888);
            frameProvider->setCurrentFrame(img);
            delete vf;
        } else {
            reader.reopen();
        }
    });
    timer->start(33); // ~30fps


    const QUrl url(QStringLiteral("qrc:/Test/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
