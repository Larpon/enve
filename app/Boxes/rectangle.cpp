#include "Boxes/rectangle.h"
#include "canvas.h"
#include "Animators/gradientpoints.h"
#include "MovablePoints/animatedpoint.h"
#include "PropertyUpdaters/nodepointupdater.h"
#include "Animators/transformanimator.h"
#include "Animators/effectanimators.h"

Rectangle::Rectangle() : PathBox(TYPE_RECTANGLE) {
    setName("Rectangle");

    mTopLeftAnimator = SPtrCreate(QPointFAnimator)("top left");
    mTopLeftPoint = SPtrCreate(AnimatedPoint)(
                mTopLeftAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);
    mTopLeftPoint->setRelativePos(QPointF(0., 0.));

    mBottomRightAnimator = SPtrCreate(QPointFAnimator)("bottom left");
    mBottomRightPoint = SPtrCreate(AnimatedPoint)(
                mTopLeftAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);
    mBottomRightPoint->setRelativePos(QPointF(0., 0.));

    //mTopLeftPoint->setBottomRightPoint(mBottomRightPoint);
    //mBottomRightPoint->setRadiusPoint(mRadiusPoint);

    ca_addChildAnimator(mTopLeftAnimator);
    ca_addChildAnimator(mBottomRightAnimator);

    ca_prependChildAnimator(mTopLeftAnimator.get(), mEffectsAnimators);
    ca_prependChildAnimator(mBottomRightAnimator.get(), mEffectsAnimators);

    mRadiusAnimator = SPtrCreate(QPointFAnimator)("round radius");
    mRadiusAnimator->setValuesRange(0., 9999.);

    mRadiusPoint = SPtrCreate(AnimatedPoint)(
                mRadiusAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);

    ca_addChildAnimator(mRadiusAnimator);
    ca_prependChildAnimator(mRadiusAnimator.get(), mEffectsAnimators);

    prp_setInheritedUpdater(SPtrCreate(NodePointUpdater)(this));
}

void Rectangle::startAllPointsTransform() {
    mTopLeftPoint->startTransform();
    mBottomRightPoint->startTransform();
    startTransform();
}

void Rectangle::finishAllPointsTransform() {
    mTopLeftPoint->finishTransform();
    mBottomRightPoint->finishTransform();
    finishTransform();
}

SkPath Rectangle::getPathAtRelFrameF(const qreal &relFrame) {
    SkPath path;
    const SkPoint topLeft =
            toSkPoint(mTopLeftAnimator->
                                getEffectiveValueAtRelFrame(relFrame));
    const SkPoint bottomRight =
            toSkPoint(mBottomRightAnimator->
                                getEffectiveValueAtRelFrame(relFrame));
    const QPointF radiusAtFrame =
            mRadiusAnimator->getEffectiveValueAtRelFrame(relFrame);
    path.addRoundRect(SkRect::MakeLTRB(topLeft.x(), topLeft.y(),
                                       bottomRight.x(), bottomRight.y()),
                      radiusAtFrame.x(), radiusAtFrame.y());
    return path;
}

void Rectangle::setTopLeftPos(const QPointF &pos) {
    mTopLeftPoint->setRelativePos(pos);
}

void Rectangle::setBottomRightPos(const QPointF &pos) {
    mBottomRightPoint->setRelativePos(pos);
}

void Rectangle::setYRadius(const qreal &radiusY) {
    mRadiusAnimator->getYAnimator()->setCurrentBaseValue(radiusY);
}

void Rectangle::setXRadius(const qreal &radiusX) {
    mRadiusAnimator->getXAnimator()->setCurrentBaseValue(radiusX);
}

void Rectangle::moveSizePointByAbs(const QPointF &absTrans) {
    mBottomRightPoint->moveByAbs(absTrans);
}

MovablePoint *Rectangle::getBottomRightPoint() {
    return mBottomRightPoint.get();
}

void Rectangle::getMotionBlurProperties(QList<Property*> &list) const {
    PathBox::getMotionBlurProperties(list);
    list.append(mTopLeftAnimator.get());
    list.append(mBottomRightAnimator.get());
    list.append(mRadiusAnimator.get());
}

bool Rectangle::differenceInEditPathBetweenFrames(
        const int& frame1, const int& frame2) const {
    if(mTopLeftAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    if(mBottomRightAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    return mRadiusAnimator->prp_differencesBetweenRelFrames(frame1, frame2);
}
