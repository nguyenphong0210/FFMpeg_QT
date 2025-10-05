#include "VideoFrameProvider.h"

VideoFrameProvider::VideoFrameProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

VideoFrameProvider::~VideoFrameProvider() {}

void VideoFrameProvider::setCurrentFrame(const QImage& img) {
    std::lock_guard<std::mutex> lock(mutex);
    currentFrame = img.copy();
}

QImage VideoFrameProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    std::lock_guard<std::mutex> lock(mutex);
    if (size)
        *size = currentFrame.size();
    return currentFrame;
}
