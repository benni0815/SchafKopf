#include "schafkopfevent.h"

SchafKopfEvent::SchafKopfEvent( t_EventData* data )
 : QEvent( QEvent::Type( QEvent::User + SCHAFKOPF_EVENT ) )
{
    m_data = data;
}

SchafKopfEvent::~SchafKopfEvent()
{
}

t_EventData* SchafKopfEvent::data()
{
    return m_data;
}
