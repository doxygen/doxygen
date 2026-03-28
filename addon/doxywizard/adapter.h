/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef ADAPTER_H
#define ADAPTER_H

#include <memory>
#include <QtGlobal>
#include <QString>
#include <QTextStream>
#include <QPointF>
#include <QMouseEvent>

/** @file
 *  @brief compatibility adapters for Qt5/Qt6 support.
 */

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
class TextCodecAdapter
{
  public:
    TextCodecAdapter(const QByteArray &name)
    {
      m_codec = QTextCodec::codecForName(name);
      if (m_codec==nullptr) // fallback: use UTF-8
      {
        m_codec = QTextCodec::codecForName("UTF-8");
      }
    }
    QByteArray encode(const QString    &input) { return m_codec ? m_codec->fromUnicode(input) : input.toLatin1(); }
    QString    decode(const QByteArray &input) { return m_codec ? m_codec->toUnicode(input)   : QString::fromLatin1(input); }
    void applyToStream(QTextStream &t) { t.setCodec(m_codec); }
    bool isValid() const { return m_codec!=nullptr; }
  private:
    QTextCodec *m_codec = nullptr; // object is owned by Qt
};
#else // Qt6+
#include <QStringEncoder>
#include <QStringDecoder>
#include <QStringConverter>
class TextCodecAdapter
{
  public:
    TextCodecAdapter(const QByteArray &name)
    {
      auto encodingOpt = QStringConverter::encodingForName(name);
      if (encodingOpt)
      {
        m_encoding = *encodingOpt;
      }
      m_encoder = std::make_unique<QStringEncoder>(m_encoding);
      m_decoder = std::make_unique<QStringDecoder>(m_encoding);
    }
    QByteArray encode(const QString &input) { return m_encoder ? m_encoder->encode(input) : input.toLatin1(); }
    QString decode(const QByteArray &input) { return m_decoder ? m_decoder->decode(input) : QString::fromLatin1(input); }
    void applyToStream(QTextStream &t) { t.setEncoding(m_encoding); }
    bool isValid() const { return m_decoder!=0; }
  private:
    std::unique_ptr<QStringEncoder> m_encoder;
    std::unique_ptr<QStringDecoder> m_decoder;
    QStringConverter::Encoding m_encoding = QStringConverter::Utf8;

};
#endif

inline qreal getMouseYPositionFromEvent(QMouseEvent *m)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  return m->y();
#else
  return m->position().y();
#endif
}


#endif
