/****************************************************************************
** Meta object code from reading C++ file 'circularindicator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../TrackerRT_UI/circularindicator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'circularindicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CircularIndicator_t {
    QByteArrayData data[8];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CircularIndicator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CircularIndicator_t qt_meta_stringdata_CircularIndicator = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CircularIndicator"
QT_MOC_LITERAL(1, 18, 11), // "setProgress"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 3), // "val"
QT_MOC_LITERAL(4, 35, 10), // "setCaption"
QT_MOC_LITERAL(5, 46, 7), // "caption"
QT_MOC_LITERAL(6, 54, 12), // "setFrequency"
QT_MOC_LITERAL(7, 67, 4) // "freq"

    },
    "CircularIndicator\0setProgress\0\0val\0"
    "setCaption\0caption\0setFrequency\0freq"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CircularIndicator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    1,   32,    2, 0x0a /* Public */,
       6,    1,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void CircularIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CircularIndicator *_t = static_cast<CircularIndicator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setCaption((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setFrequency((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CircularIndicator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CircularIndicator.data,
      qt_meta_data_CircularIndicator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CircularIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CircularIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CircularIndicator.stringdata0))
        return static_cast<void*>(const_cast< CircularIndicator*>(this));
    return QWidget::qt_metacast(_clname);
}

int CircularIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
