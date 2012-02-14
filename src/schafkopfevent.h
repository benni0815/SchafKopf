#include "schafkopfdef.h"

#include <QEvent>
#include <QObject>

class SchafKopfEvent : public QObject, public QEvent
{
    Q_OBJECT
public:
    SchafKopfEvent( t_EventData* data );
    ~SchafKopfEvent();

    t_EventData* data();

private:
    t_EventData *m_data;
};

