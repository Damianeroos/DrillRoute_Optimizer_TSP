
#pragma once
#include <QPointF>
#include <QString>

// Parsuje atrybut rotacji np. "R90", "MR0"
double parseRotationDegrees(const QString& rotAttr, bool* mirrored = nullptr);

// Obraca punkt wokół (0,0) o kąt w radianach
QPointF rotatePoint(const QPointF& p, double radians);
