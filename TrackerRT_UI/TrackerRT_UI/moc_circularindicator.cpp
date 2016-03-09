/****************************************************************************
** Meta object code from reading C++ file 'circularindicator.h'
**
** Created: Wed Mar 9 11:27:51 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../circularindicator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'circularindicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CircularIndicator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   19,   18,   18, 0x0a,
      48,   40,   18,   18, 0x0a,
      73,   68,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CircularIndicator[] = {
    "CircularIndicator\0\0val\0setProgress(int)\0"
    "caption\0setCaption(QString)\0freq\0"
    "setFrequency(int)\0"
};

void CircularIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CircularIndicator *_t = static_cast<CircularIndicator *>(_o);
        switch (_id) {
        case 0: _t->setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setCaption((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setFrequency((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CircularIndicator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CircularIndicator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CircularIndicator,
      qt_meta_data_CircularIndicator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CircularIndicator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CircularIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CircularIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CircularIndicator))
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
