#ifndef VIDEOFRAMEPROVIDER_H
#define VIDEOFRAMEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <mutex>

class VideoFrameProvider : public QQuickImageProvider {
public:
    VideoFrameProvider();
    ~VideoFrameProvider();

    void setCurrentFrame(const QImage& img);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QImage currentFrame;
    std::mutex mutex;
};

#endif // VIDEOFRAMEPROVIDER_H
