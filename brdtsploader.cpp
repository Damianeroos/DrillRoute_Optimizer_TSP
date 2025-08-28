#include "brdtsploader.h"
#include "helpers.h"

#include <QFile>
#include <QHash>
#include <QList>
#include <QDebug>

namespace{
using Pads = QVector<QPointF>;
using PackageIndex = QHash<QString, Pads>;
using LibraryIndex = QHash<QString, PackageIndex>;

static LibraryIndex buildLibraryIndex(const QDomElement& libraries)
{
    LibraryIndex idx;

    for (QDomElement lib = libraries.firstChildElement("library");
         !lib.isNull(); lib = lib.nextSiblingElement("library"))
    {
        const QString urn  = lib.attribute("urn");
        const QString name = lib.attribute("name");
        const QString key  = !urn.isEmpty() ? urn : name;
        if (key.isEmpty()) continue;

        PackageIndex& pindex = idx[key];

        for (QDomElement pkgs = lib.firstChildElement("packages");
             !pkgs.isNull(); pkgs = pkgs.nextSiblingElement("packages"))
        {
            for (QDomElement pkg = pkgs.firstChildElement("package");
                 !pkg.isNull(); pkg = pkg.nextSiblingElement("package"))
            {
                const QString pkgName = pkg.attribute("name");
                if (pkgName.isEmpty()) continue;

                Pads pads;
                for (QDomElement pad = pkg.firstChildElement("pad");
                     !pad.isNull(); pad = pad.nextSiblingElement("pad"))
                {
                    if (!pad.hasAttribute("x") || !pad.hasAttribute("y")) continue;

                    pads.push_back(QPointF(pad.attribute("x").toDouble(),
                                           pad.attribute("y").toDouble()));
                }
                if (!pads.isEmpty())
                    pindex.insert(pkgName, std::move(pads));
            }
        }
    }

    return idx;
}

} //namespace

TspProblem brdTspLoader::load(const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("Cannot open file");

    QDomDocument doc;
    if (!doc.setContent(&f))
        throw std::runtime_error("Invalid XML");
    f.close();

    TspProblem tsp;

    const auto holes = readHoles(doc);
    if (holes.isEmpty())
        throw std::runtime_error("Loaded board is empty.");

    QVector<double> X, Y;
    X.reserve(holes.size());
    Y.reserve(holes.size());
    for (const auto& p : holes) {
        X.push_back(p.x());
        Y.push_back(p.y());
    }

    tsp.setPointX(std::move(X));
    tsp.setPointY(std::move(Y));
    tsp.buildDistanceMatrix();

    return tsp;
}

QVector<QPointF> brdTspLoader::readHoles(const QDomDocument &doc)
{
    const QDomElement root      = doc.documentElement();
    const QDomElement drawing   = root.firstChildElement("drawing");
    const QDomElement board     = drawing.firstChildElement("board");
    const QDomElement elements  = board.firstChildElement("elements");
    const QDomElement libraries = board.firstChildElement("libraries");

    if (drawing.isNull() || board.isNull() || elements.isNull() || libraries.isNull()) {
        qWarning() << "readHoles: missing drawing/board/elements/libraries";
        return {};
    }

    const LibraryIndex libIndex = buildLibraryIndex(libraries);

    QVector<QPointF> holes;
    holes.reserve(256);

    for (QDomElement el = elements.firstChildElement("element");
         !el.isNull(); el = el.nextSiblingElement("element"))
    {
        const QString libKey = el.attribute("library_urn",
                                            el.attribute("library"));
        const QString pkgName = el.attribute("package");
        if (pkgName.isEmpty()) continue;

        const QString rotAttr = el.attribute("rot", "R0");
        const double ex = el.attribute("x").toDouble();
        const double ey = el.attribute("y").toDouble();

        auto pit = libIndex.constFind(libKey);
        if (pit == libIndex.constEnd()) {
            const QString libName = el.attribute("library");
            if (!libName.isEmpty())
                pit = libIndex.constFind(libName);
            if (pit == libIndex.constEnd())
                continue;
        }

        const auto& pindex = *pit;
        const auto pkgIt = pindex.constFind(pkgName);
        if (pkgIt == pindex.constEnd())
            continue;

        bool mirrored = false;
        const double deg = parseRotationDegrees(rotAttr, &mirrored);
        const double rad = qDegreesToRadians(deg);

        for (const QPointF& local : pkgIt.value()) {
            QPointF p = local;
            if (mirrored) p.setX(-p.x());

            p = rotatePoint(p, rad);
            p.rx() += ex;
            p.ry() += ey;

            holes.push_back(p);
        }
    }

    if (holes.isEmpty())
        qWarning() << "readHoles: no pads matched (sprawdź library_urn/library, urn/name, package).";

            return holes;
}

