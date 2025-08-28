#include "helpers.h"
#include <QtMath>

double parseRotationDegrees(const QString& rotAttr, bool* mirrored)
{
    if (mirrored)
        *mirrored = rotAttr.startsWith('M');

    const int rIndex = rotAttr.indexOf('R');
    if (rIndex < 0)
        return 0.0;

    bool ok = false;
    const double deg = rotAttr.mid(rIndex + 1).toDouble(&ok);
    return ok ? deg : 0.0;
}

QPointF rotatePoint(const QPointF& p, double radians)
{
    const double c = std::cos(radians);
    const double s = std::sin(radians);
    return QPointF(p.x() * c - p.y() * s,
                   p.x() * s + p.y() * c);
}
