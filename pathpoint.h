#ifndef PATHPOINT_H
#define PATHPOINT_H
#include "movablepoint.h"
#include <QSqlQuery>
#include "pointhelpers.h"

class UndoRedoStack;

class VectorPath;

class CtrlPoint;

enum CanvasMode : short;

class PathPoint;

class PathPointAnimators : public ComplexAnimator {
public:
    PathPointAnimators() : ComplexAnimator() {
        setName("point");
    }

    void setAllVars(PathPoint *parentPathPointT,
                       QPointFAnimator *endPosAnimatorT,
                       QPointFAnimator *startPosAnimatorT,
                       QPointFAnimator *pathPointPosAnimatorT) {
        parentPathPoint = parentPathPointT;
        endPosAnimator = endPosAnimatorT;
        endPosAnimator->setName("ctrl pt 1 pos");
        startPosAnimator = startPosAnimatorT;
        startPosAnimator->setName("ctrl pt 2 pos");
        pathPointPosAnimator = pathPointPosAnimatorT;
        pathPointPosAnimator->setName("point pos");

        addChildAnimator(pathPointPosAnimator);
        addChildAnimator(endPosAnimator);
        addChildAnimator(startPosAnimator);
    }

    bool isOfPathPoint(PathPoint *checkPoint) {
        return parentPathPoint == checkPoint;
    }

private:
    PathPoint *parentPathPoint;
    QPointFAnimator *endPosAnimator;
    QPointFAnimator *startPosAnimator;
    QPointFAnimator *pathPointPosAnimator;
};

class PathPoint : public MovablePoint
{
public:
    PathPoint(QPointF absPos, VectorPath *vectorPath);
    PathPoint(QPointF absPos, QPointF startCtrlAbsPos, QPointF endCtrlAbsPos,
              VectorPath *vectorPath);
    PathPoint(int movablePointId, int pathPointId, VectorPath *vectorPath);

    void startTransform();
    void finishTransform();

    void moveBy(QPointF absTranslation);
    void moveToAbs(QPointF absPos);

    QPointF getStartCtrlPtAbsPos();
    QPointF getStartCtrlPtValue();
    CtrlPoint *getStartCtrlPt();

    QPointF getEndCtrlPtAbsPos();
    QPointF getEndCtrlPtValue();
    CtrlPoint *getEndCtrlPt();

    void draw(QPainter *p, CanvasMode mode);

    PathPoint *getNextPoint();
    PathPoint *getPreviousPoint();

    bool isEndPoint();

    void setPointAsPrevious(PathPoint *pointToSet, bool saveUndoRedo = true);
    void setPointAsNext(PathPoint *pointToSet, bool saveUndoRedo = true);
    void setNextPoint(PathPoint *mNextPoint, bool saveUndoRedo = true);
    void setPreviousPoint(PathPoint *mPreviousPoint, bool saveUndoRedo = true);

    bool hasNextPoint();
    bool hasPreviousPoint();

    PathPoint *addPointAbsPos(QPointF absPos);
    PathPoint *addPoint(PathPoint *pointToAdd);

    void connectToPoint(PathPoint *point);
    void disconnectFromPoint(PathPoint *point);

    void remove();

    MovablePoint *getPointAtAbsPos(QPointF absPos, CanvasMode canvasMode);
    void rectPointsSelection(QRectF absRect, QList<MovablePoint *> *list);
    void updateStartCtrlPtVisibility();
    void updateEndCtrlPtVisibility();

    void setSeparatePathPoint(bool separatePathPoint);
    bool isSeparatePathPoint();

    void setCtrlsMode(CtrlsMode mode, bool saveUndoRedo = true);
    QPointF symmetricToAbsPos(QPointF absPosToMirror);
    QPointF symmetricToAbsPosNewLen(QPointF absPosToMirror, qreal newLen);
    void ctrlPointPosChanged(bool startPtChanged);
    void moveEndCtrlPtToAbsPos(QPointF endCtrlPt);
    void moveStartCtrlPtToAbsPos(QPointF startCtrlPt);
    void moveEndCtrlPtToRelPos(QPointF endCtrlPt);
    void moveStartCtrlPtToRelPos(QPointF startCtrlPt);
    void setCtrlPtEnabled(bool enabled, bool isStartPt, bool saveUndoRedo = true);
    VectorPath *getParentPath();

    void saveToSql(int vectorPathId);
    PathPoint(qreal relPosX, qreal relPosy,
              qreal startCtrlRelX, qreal startCtrlRelY,
              qreal endCtrlRelX, qreal endCtrlRelY, bool isFirst, int boneZ, VectorPath *vectorPath);

    void clearAll();
    void cancelTransform();
    void attachToBoneFromSqlZId();

    void setEndCtrlPtEnabled(bool enabled);
    void setStartCtrlPtEnabled(bool enabled);

    bool isEndCtrlPtEnabled();
    bool isStartCtrlPtEnabled();

    void setPosAnimatorUpdater(AnimatorUpdater *updater);

    void updateAfterFrameChanged(int frame);

    PathPointAnimators *getPathPointAnimatorsPtr();
    void setPointId(int idT);

private:
    int mPointId;
    PathPointAnimators mPathPointAnimators;

    VectorPath *mVectorPath;
    CtrlsMode mCtrlsMode = CtrlsMode::CTRLS_SYMMETRIC;

    bool mSeparatePathPoint = false;
    PathPoint *mNextPoint = NULL;
    PathPoint *mPreviousPoint = NULL;
    bool mStartCtrlPtEnabled = false;
    CtrlPoint *mStartCtrlPt;
    bool mEndCtrlPtEnabled = false;
    CtrlPoint *mEndCtrlPt;
    void ctrlPointPosChanged(CtrlPoint *pointChanged, CtrlPoint *pointToUpdate);
};

#endif // PATHPOINT_H
