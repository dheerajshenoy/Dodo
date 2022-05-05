/****************************************************************************
** Meta object code from reading C++ file 'Dodo.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../src/Dodo.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Dodo.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Dodo_t {
    const uint offsetsAndSize[18];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Dodo_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Dodo_t qt_meta_stringdata_Dodo = {
    {
QT_MOC_LITERAL(0, 4), // "Dodo"
QT_MOC_LITERAL(5, 18), // "currentPageChanged"
QT_MOC_LITERAL(24, 0), // ""
QT_MOC_LITERAL(25, 4), // "page"
QT_MOC_LITERAL(30, 15), // "documentChanged"
QT_MOC_LITERAL(46, 18), // "Poppler::Document*"
QT_MOC_LITERAL(65, 3), // "doc"
QT_MOC_LITERAL(69, 11), // "pageChanged"
QT_MOC_LITERAL(81, 21) // "handleDocumentChanged"

    },
    "Dodo\0currentPageChanged\0\0page\0"
    "documentChanged\0Poppler::Document*\0"
    "doc\0pageChanged\0handleDocumentChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Dodo[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       1,    0,   47,    2, 0x26,    3 /* Public | MethodCloned */,
       4,    1,   48,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   51,    2, 0x08,    6 /* Private */,
       8,    1,   54,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void Dodo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Dodo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentPageChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->currentPageChanged(); break;
        case 2: _t->documentChanged((*reinterpret_cast< std::add_pointer_t<Poppler::Document*>>(_a[1]))); break;
        case 3: _t->pageChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->handleDocumentChanged((*reinterpret_cast< std::add_pointer_t<Poppler::Document*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Dodo::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Dodo::currentPageChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Dodo::*)(Poppler::Document * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Dodo::documentChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Dodo::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_Dodo.offsetsAndSize,
    qt_meta_data_Dodo,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Dodo_t
, QtPrivate::TypeAndForceComplete<Dodo, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Poppler::Document *, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Poppler::Document *, std::false_type>


>,
    nullptr
} };


const QMetaObject *Dodo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dodo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Dodo.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Dodo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Dodo::currentPageChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void Dodo::documentChanged(Poppler::Document * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
