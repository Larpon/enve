#include "animatorupdater.h"
#include <QDebug>
#include "vectorpath.h"
#include "mainwindow.h"

TransUpdater::TransUpdater(BoundingBox *boundingBox) : AnimatorUpdater()
{
    mTarget = boundingBox;
}

void TransUpdater::update() {
    mTarget->updateCombinedTransform();
    mTarget->schedulePivotUpdate();
}

PathPointUpdater::PathPointUpdater(VectorPath *vectorPath)
{
    mTarget = vectorPath;
}

void PathPointUpdater::update()
{
    mTarget->schedulePathUpdate();
}

GradientUpdater::GradientUpdater(Gradient *gradient)
{
    mTarget = gradient;
}

void GradientUpdater::update()
{
    mTarget->scheduleQGradientStopsUpdate();
    MainWindow::getInstance()->scheduleDisplayedFillStrokeSettingsUpdate();
}

StrokeWidthUpdater::StrokeWidthUpdater(VectorPath *path)
{
    mTarget = path;
}

void StrokeWidthUpdater::update()
{
    mTarget->scheduleOutlinePathUpdate();
    MainWindow::getInstance()->scheduleDisplayedFillStrokeSettingsUpdate();
}

DisplayedFillStrokeSettingsUpdater::DisplayedFillStrokeSettingsUpdater()
{

}

void DisplayedFillStrokeSettingsUpdater::update()
{
    MainWindow::getInstance()->scheduleDisplayedFillStrokeSettingsUpdate();
}
