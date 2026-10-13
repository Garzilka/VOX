#ifndef NEWMESSAGE_H
#define NEWMESSAGE_H

#include <QEvent>
#include <QString>


const QEvent::Type NewMessageEvent = static_cast<QEvent::Type>(QEvent::User + 1);

class NewMessage : public QEvent
{
public:
    NewMessage(const QString &message)
        : QEvent(NewMessageEvent), m_message(message) {}

    QString message() const { return m_message; }

private:
    QString m_message = "";
};


#endif // NEWMESSAGE_H
