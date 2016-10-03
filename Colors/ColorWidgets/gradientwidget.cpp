#include "gradientwidget.h"
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include "Colors/helpers.h"

GradientWidget::GradientWidget(QWidget *parent, MainWindow *mainWindow) : GLWidget(parent)
{
    mMainWindow = mainWindow;
    setFixedHeight(6*20 + 10);
//    newGradient();
//    newGradient(Color(1.f, 1.f, 0.f), Color(0.f, 1.f, 1.f, 0.5f));
//    newGradient(Color(1.f, 0.f, 0.f), Color(0.f, 1.f, 0.f));
}

GradientWidget::~GradientWidget()
{
    foreach(Gradient *gradient, mGradients) {
        delete gradient;
    }
}

void GradientWidget::setCurrentColorId(int id) {
    mCurrentColorId = id;
    Color col = mCurrentGradient->getCurrentColorAt(mCurrentColorId);
    emit selectedColorChanged(col.gl_h, col.gl_s, col.gl_v, col.gl_a);
    repaint();
}

void GradientWidget::addGradientToList(Gradient *gradient) {
    mGradients << gradient;
    gradient->incNumberPointers();
}

void GradientWidget::newGradient(Color color1, Color color2) {
    Gradient *newGradient = new Gradient(color1, color2, this);
    addGradientToList(newGradient);
    setCurrentGradient(newGradient);
    repaint();
}

void GradientWidget::newGradient(int fromGradientId)
{
    Gradient *fromGradient = mGradients.at(fromGradientId);
    Gradient *newGradient = new Gradient(fromGradient, this);
    addGradientToList(newGradient);
    setCurrentGradient(mGradients.last());
    repaint();
}

void GradientWidget::clearAll() {
    foreach(Gradient *gradient, mGradients) {
        gradient->decNumberPointers();
    }
    mGradients.clear();

    mCurrentGradient = NULL;
    mCenterGradientId = 1;
    mCurrentColorId = 0;
    repaint();
}

void GradientWidget::removeGradientFromList(Gradient *toRemove) {
    mGradients.removeOne(toRemove);
    if(mCurrentGradient == toRemove) {
        setCurrentGradient((Gradient*) NULL);
    }
    toRemove->decNumberPointers();
}

void GradientWidget::removeGradient(int gradientId)
{
    Gradient *toRemove = mGradients.at(gradientId);
    if(toRemove->affectsPaths()) {
        return;
    }
    removeGradientFromList(toRemove);

    mCenterGradientId = clampInt(mCenterGradientId, 1, mGradients.length() - 2);
    repaint();
}

void GradientWidget::saveGradientsToQuery() {
    foreach(Gradient *gradient, mGradients) {
        gradient->saveToSql();
    }
}

void GradientWidget::saveGradientsToSqlIfPathSelected() {
    foreach(Gradient *gradient, mGradients) {
        gradient->saveToSqlIfPathSelected();
    }
}

void GradientWidget::loadAllGradientsFromSql() {
    foreach(Gradient *gradient, mGradients) {
        gradient->setSqlId(-1);
    }

    QSqlQuery query;
    QString queryStr = QString("SELECT * FROM gradient");
    if(query.exec(queryStr) ) {
        int idId = query.record().indexOf("id");
        while(query.next() ) {
            addGradientToList(new Gradient(query.value(idId).toInt(), this) );
        }
        if(mGradients.isEmpty()) return;
        setCurrentGradient(mGradients.last());
        repaint();
    } else {
        qDebug() << "Could not load gradients";
    }
}

Gradient *GradientWidget::getGradientBySqlId(int id) {
    foreach (Gradient *gradient, mGradients) {
        if(gradient->getSqlId() == id) {
            return gradient;
        }
    }
    return NULL;
}

void GradientWidget::wheelEvent(QWheelEvent *event)
{
    if(event->y() > height()/2 || mGradients.length() < 4) return;
    if(event->delta() < 0) {
        mCenterGradientId = clampInt(mCenterGradientId + 1, 1, mGradients.length() - 2);
    } else {
        mCenterGradientId = clampInt(mCenterGradientId - 1, 1, mGradients.length() - 2);
    }
    repaint();
}

void GradientWidget::setCurrentColor(GLfloat h, GLfloat s, GLfloat v, GLfloat a)
{
    Color newColor;
    newColor.setHSV(h, s, v, a);
    mCurrentGradient->replaceColor(mCurrentColorId, newColor);
    repaint();
}

void GradientWidget::setCurrentGradient(Gradient *gradient)
{
    if(gradient == NULL) {
        if(mGradients.isEmpty()) newGradient();
        setCurrentGradient(0);
        return;
    } else if(mCurrentGradient == gradient) {
        return;
    }
    mCurrentGradient = gradient;
    setCurrentColorId(0);
    repaint();
}

Gradient *GradientWidget::getCurrentGradient()
{
    return mCurrentGradient;
}

Color GradientWidget::getCurrentColor()
{
    if(mCurrentGradient == NULL) return Color();
    return mCurrentGradient->getCurrentColorAt(mCurrentColorId);
}

void GradientWidget::setCurrentGradient(int listId)
{
    if(listId >= mGradients.length()) return;
    setCurrentGradient(mGradients.at(listId));
    emit currentGradientChanged(mCurrentGradient);
}

void GradientWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->y() < height()/2 - 10) {
        int heightGrad = (height()/2 - 10)/3;
        int gradId = event->y()/heightGrad + mCenterGradientId - 1;
        bool gradPressed = true;
        if(gradId >= mGradients.count() || gradId < 0) {
            gradPressed = false;
        }
        if(event->button() == Qt::RightButton) {
            QMenu menu(this);
            menu.addAction("New Gradient");
            if(gradPressed) {
                menu.addAction("Duplicate Gradient");
                menu.addAction("Delete Gradient");
            }
            QAction *selected_action = menu.exec(event->globalPos());
            if(selected_action != NULL)
            {
                if(selected_action->text() == "Delete Gradient")
                {
                    removeGradient(gradId);
                } else if(selected_action->text() == "Duplicate Gradient") {
                    newGradient(gradId);
                } else if(selected_action->text() == "New Gradient"){
                    newGradient();
                }
            } else {

            }
        } else if(gradPressed) {
            setCurrentGradient(gradId);
        }
    } else if(event->y() > height()/2 && event->y() < 3*height()/4 && mCurrentGradient != NULL) {
        int nCols = mCurrentGradient->getColorCount();
        setCurrentColorId(clampInt(event->x()*nCols/width(), 0, nCols - 1) );
        if(event->button() == Qt::RightButton)
        {
            QMenu menu(this);
            menu.addAction("Delete Color");
            menu.addAction("Add Color");
            QAction *selected_action = menu.exec(event->globalPos());
            if(selected_action != NULL)
            {
                if(selected_action->text() == "Delete Color")
                {

                    if(mCurrentGradient->getColorCount() < 2) {
                        mCurrentGradient->replaceColor(mCurrentColorId,
                                                       Color(0.f, 0.f, 0.f, 1.f));
                    } else {
                        mCurrentGradient->removeColor(mCurrentColorId);
                    }
                    startGradientTransform();
                    setCurrentColorId(0);
                    emit gradientSettingsChanged();
                    finishGradientTransform();
                    repaint();
                } else if(selected_action->text() == "Add Color") {
                    startGradientTransform();
                    mCurrentGradient->addColor(Color(0.f, 0.f, 0.f, 1.f));
                    emit gradientSettingsChanged();
                    finishGradientTransform();
                    repaint();
                }
            }
            else
            {

            }
        }
    }
}

void GradientWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->y() > height()/2 && event->y() < 3*height()/4 && mCurrentGradient != NULL) {
        int nCols = mCurrentGradient->getColorCount();
            int colorId = clampInt(event->x()*nCols/width(), 0, nCols - 1);
            if(colorId != mCurrentColorId) {
                startGradientTransform();
                mCurrentGradient->swapColors(mCurrentColorId, colorId);
                setCurrentColorId(colorId);
                emit gradientSettingsChanged();
                finishGradientTransform();
                repaint();
            }
    }
}

void GradientWidget::drawBorder(GLfloat xt, GLfloat yt, GLfloat wt, GLfloat ht) {
    drawRect(xt, yt, wt, 2.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             false, false, false, false);
    drawRect(xt, yt + ht - 2.f, wt, 2.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             false, false, false, false);
    drawRect(xt, yt + 2.f, 2.f, ht - 4.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             false, false, false, false);
    drawRect(xt + wt - 2.f, yt + 2.f, 2.f, ht - 4.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             0.f, 0.f, 0.f, 1.f,
             false, false, false, false);

    xt += 2.f;
    yt += 2.f;
    wt -= 4.f;
    ht -= 4.f;
    drawRect(xt, yt, wt, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             false, false, false, false);
    drawRect(xt, yt + ht - 1.f, wt, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             false, false, false, false);
    drawRect(xt, yt + 1.f, 1.f, ht - 2.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             false, false, false, false);
    drawRect(xt + wt - 1.f, yt + 1.f, 1.f, ht - 2.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             1.f, 1.f, 1.f, 1.f,
             false, false, false, false);
}

void GradientWidget::drawGradient(int id, GLfloat height, GLfloat cY, bool border) {
    Gradient *gradient = mGradients.at(id);

    int len = gradient->getColorCount();
    Color nextColor = gradient->getCurrentColorAt(0);
    GLfloat cX = 0.f;
    GLfloat segWidth = width()/(GLfloat)(len - 1);

    for(int i = 0; i < len - 1; i++) {
        Color color = nextColor;
        nextColor = gradient->getCurrentColorAt(i + 1);

        drawRect(cX, cY, segWidth, height,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 nextColor.gl_r, nextColor.gl_g, nextColor.gl_b, nextColor.gl_a,
                 nextColor.gl_r, nextColor.gl_g, nextColor.gl_b, nextColor.gl_a,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 false, false, false, false);
        cX += segWidth;
    }
    if(gradient == mCurrentGradient && border) {
        drawBorder(0.f, cY, width(), height);
    }
}

void GradientWidget::paintGL()
{
    if(mCurrentGradient == NULL) return;
    GLfloat halfHeight = height()*0.5f;
    GLfloat quorterHeight = halfHeight*0.5f;
    GLfloat scrollItemHeight = (halfHeight - 10.f)*0.334f;

    GLfloat val1 = 0.5f;
    GLfloat val2 = 0.25f;
    for(int i = 0; i < width(); i += 7) {
        for(int j = 0; j < height(); j += 7) {
            GLfloat val = ((i + j) % 2 == 0) ? val1 : val2;
            drawSolidRect(i, j, 7, 7, val, val, val,
                          false, false, false, false);
        }
    }

    for(int i = (mCenterGradientId > 1) ? mCenterGradientId - 1 : 0;
        i < mCenterGradientId + 2 && i < mGradients.length();
        i++) {
        drawGradient(i, scrollItemHeight, (i - mCenterGradientId + 1)*scrollItemHeight);
    }
    drawSolidRect(0.f, halfHeight - 10.f, width(), 10.f, 1.f, 1.f, 1.f, 1.f,
                  false, false, false, false);

    int len = mCurrentGradient->getColorCount();
    Color nextColor = mCurrentGradient->getCurrentColorAt(0);
    GLfloat cX = 0.f;
    GLfloat segWidth = width()/(GLfloat)len;
    for(int i = 0; i < len; i++) {
        Color color = nextColor;
        drawRect(cX, halfHeight, segWidth, quorterHeight,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 color.gl_r, color.gl_g, color.gl_b, color.gl_a,
                 false, false, false, false);
        if(i == mCurrentColorId) {
            drawBorder(cX, halfHeight, segWidth, quorterHeight);
        }
        if(i + 1 == len) break;
        nextColor = mCurrentGradient->getCurrentColorAt(i + 1);
        cX += segWidth;
    }
    drawGradient(mGradients.indexOf(mCurrentGradient),
                 quorterHeight, halfHeight + quorterHeight, false);
}

void GradientWidget::startSelectedColorTransform() {
    if(mCurrentGradient == NULL) return;
    mCurrentGradient->startColorIdTransform(mCurrentColorId);
}

void GradientWidget::finishGradientTransform()
{
    if(mCurrentGradient == NULL) return;
    mCurrentGradient->finishTransform();
}

void GradientWidget::startGradientTransform()
{
    if(mCurrentGradient == NULL) return;
    mCurrentGradient->startTransform();
}
