#ifndef PIXMAPEFFECT_H
#define PIXMAPEFFECT_H
#include "skia/skiaincludes.h"
#include "Animators/staticcomplexanimator.h"
#include <QMimeData>
#include <QPointF>
class EffectAnimators;
class QIODevice;
class BoundingBoxRenderData;

struct PixmapEffectRenderData : public StdSelfRef {
    virtual void applyEffectsSk(const SkBitmap &bitmap,
                                const qreal scale) = 0;
};

class PixmapEffect;

typedef PropertyMimeData<PixmapEffect,
    InternalMimeData::PIXMAP_EFFECT> PixmapEffectMimeData;

enum PixmapEffectType : short {
    EFFECT_BLUR,
    EFFECT_SHADOW,
    EFFECT_LINES,
    EFFECT_CIRCLES,
    EFFECT_SWIRL,
    EFFECT_DESATURATE,
    EFFECT_OIL,
    EFFECT_IMPLODE,
    EFFECT_COLORIZE,
    EFFECT_REPLACE_COLOR,
    EFFECT_CONTRAST,
    EFFECT_BRIGHTNESS,
    EFFECT_BRUSH,
    EFFECT_MOTION_BLUR
};

class PixmapEffect : public StaticComplexAnimator {
    friend class SelfRef;
protected:
    PixmapEffect(const QString& name, const PixmapEffectType type);
public:
    virtual stdsptr<PixmapEffectRenderData> getPixmapEffectRenderDataForRelFrameF(
            const qreal relFrame, BoundingBoxRenderData * const data) = 0;
    virtual qreal getMargin();
    virtual qreal getMarginAtRelFrame(const int );

    bool SWT_isPixmapEffect() const;

    void writeProperty(QIODevice * const dst) const;
    void readProperty(QIODevice * const src);

    void prp_startDragging();

    friend QDataStream & operator << (QDataStream & s,
                                      const PixmapEffect *ptr);
    friend QDataStream & operator >> (QDataStream & s,
                                      PixmapEffect *& ptr);

    QMimeData *SWT_createMimeData();

    EffectAnimators *getParentEffectAnimators();
    void writeIdentifier(QIODevice * const dst) const;

    void switchVisible();
    void setVisible(const bool visible);
    bool isVisible() const;
protected:
    bool mVisible = true;
    PixmapEffectType mType;
};
#endif // PIXMAPEFFECT_H
