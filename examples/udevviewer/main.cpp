// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "qudev_service.h"
#include "qudev_device_model.h"
#include "qudev_device_filters_model.h"


int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("samsa");
    QCoreApplication::setApplicationName("qudevviewer");

    QudevDeviceModel deviceModel;
    QudevFiltersModel filtersModel;
    QudevService service;
    // Initially load filters if any
    service.setFilters(filtersModel.toQudevFilters());

    QObject::connect(&service, &QudevService::scanFinished, &deviceModel, &QudevDeviceModel::setDevices);
    QObject::connect(&service, &QudevService::deviceFound, &deviceModel, &QudevDeviceModel::deviceAdded);
    QObject::connect(&filtersModel, &QudevFiltersModel::changed, &service, [&](){
        service.setFilters(filtersModel.toQudevFilters());
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("service", &service);
    engine.rootContext()->setContextProperty("deviceModel",   &deviceModel);
    engine.rootContext()->setContextProperty("filtersModel", &filtersModel);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return 1;

    return app.exec();
}
