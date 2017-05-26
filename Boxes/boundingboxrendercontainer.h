#ifndef BOUNDINGBOXRENDERCONTAINER_H
#define BOUNDINGBOXRENDERCONTAINER_H
class BoundingBox;
#include <QImage>
#include "SkCanvas.h"
#include "SkImage.h"
#include "selfref.h"
#include "skqtconversions.h"

class CacheHandler;

class CacheContainer : public SimpleSmartPointer {
public:
    CacheContainer();

    virtual ~CacheContainer();

    void setParentCacheHandler(CacheHandler *handler);
    bool freeThis();

    void thisAccessed();

    int getByteCount() {
        return mImage.byteCount();
    }

    QImage getImage() {
        return mImage;
    }

    sk_sp<SkImage> getImageSk() {
        return mImageSk;
    }

    void replaceImage(const QImage &img);

    const int &getMinRelFrame() const;

    const int &getMaxRelFrame() const;

    void setRelFrame(const int &frame);
    void setMaxRelFrame(const int &maxFrame);
    void setMinRelFrame(const int &minFrame);
    void setRelFrameRange(const int &minFrame, const int &maxFrame);
    bool relFrameInRange(const int &relFrame);
    void drawCacheOnTimeline(QPainter *p,
                             const qreal &pixelsPerFrame,
                             const qreal &drawY,
                             const int &startFrame,
                             const int &endFrame);
    virtual void draw(QPainter *p);
    virtual void drawSk(SkCanvas *canvas);

    void setBlocked(const bool &bT) {
        if(bT == mBlocked) return;
        mBlocked = bT;
        if(bT) {
            incNumberPointers();
        } else {
            decNumberPointers();
        }
    }

protected:
    bool mBlocked = false;
    sk_sp<SkImage> mImageSk;
    QImage mImage;
    CacheHandler *mParentCacheHandler = NULL;
    int mMinRelFrame = 0;
    int mMaxRelFrame = 0;
};

class RenderContainer : public CacheContainer {
public:
    RenderContainer() {}
    virtual ~RenderContainer() {}

    void draw(QPainter *p);
    void drawSk(SkCanvas *canvas);

    void updatePaintTransformGivenNewCombinedTransform(
            const QMatrix &combinedTransform);

    void setTransform(const QMatrix &transform);

    void setDrawPos(const QPoint &drawpos);

    const QMatrix &getTransform() const;

    const QImage &getImage() const;

    const QMatrix &getPaintTransform() const;

    const QPoint &getDrawpos() const;

    const qreal &getResolutionFraction() const;

    void updateVariables(const QMatrix &combinedTransform,
                         const qreal &effectsMargin,
                         const qreal &resolutionPer,
                         BoundingBox *target);

    void setVariables(const QMatrix &transform,
                      const QMatrix &paintTransform,
                      const QPoint &drawpos,
                      const QImage &img,
                      const sk_sp<SkImage> &imgSk,
                      const qreal &res);
    void duplicateFrom(RenderContainer *src);
protected:
    qreal mResolutionFraction;
    QMatrix mTransform;
    QMatrix mPaintTransform;
    QPoint mDrawPos;
};

#endif // BOUNDINGBOXRENDERCONTAINER_H
