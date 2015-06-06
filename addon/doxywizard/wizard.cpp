#include "wizard.h"
#include "input.h"
#include "doxywizard.h"

#include <math.h>
#include <QtWidgets/QtWidgets>

// options configurable via the wizard
#define STR_PROJECT_NAME          QStringLiteral("PROJECT_NAME")
#define STR_PROJECT_LOGO          QStringLiteral("PROJECT_LOGO")
#define STR_PROJECT_BRIEF         QStringLiteral("PROJECT_BRIEF")
#define STR_INPUT                 QStringLiteral("INPUT")
#define STR_OUTPUT_DIRECTORY      QStringLiteral("OUTPUT_DIRECTORY")
#define STR_PROJECT_NUMBER        QStringLiteral("PROJECT_NUMBER")
#define STR_RECURSIVE             QStringLiteral("RECURSIVE")
#define STR_OPTIMIZE_OUTPUT_FOR_C QStringLiteral("OPTIMIZE_OUTPUT_FOR_C")
#define STR_OPTIMIZE_OUTPUT_JAVA  QStringLiteral("OPTIMIZE_OUTPUT_JAVA")
#define STR_OPTIMIZE_FOR_FORTRAN  QStringLiteral("OPTIMIZE_FOR_FORTRAN")
#define STR_OPTIMIZE_OUTPUT_VHDL  QStringLiteral("OPTIMIZE_OUTPUT_VHDL")
#define STR_CPP_CLI_SUPPORT       QStringLiteral("CPP_CLI_SUPPORT")
#define STR_HIDE_SCOPE_NAMES      QStringLiteral("HIDE_SCOPE_NAMES")
#define STR_EXTRACT_ALL           QStringLiteral("EXTRACT_ALL")
#define STR_SOURCE_BROWSER        QStringLiteral("SOURCE_BROWSER")
#define STR_GENERATE_HTML         QStringLiteral("GENERATE_HTML")
#define STR_GENERATE_LATEX        QStringLiteral("GENERATE_LATEX")
#define STR_GENERATE_MAN          QStringLiteral("GENERATE_MAN")
#define STR_GENERATE_RTF          QStringLiteral("GENERATE_RTF")
#define STR_GENERATE_XML          QStringLiteral("GENERATE_XML")
#define STR_GENERATE_HTMLHELP     QStringLiteral("GENERATE_HTMLHELP")
#define STR_GENERATE_TREEVIEW     QStringLiteral("GENERATE_TREEVIEW")
#define STR_USE_PDFLATEX          QStringLiteral("USE_PDFLATEX")
#define STR_PDF_HYPERLINKS        QStringLiteral("PDF_HYPERLINKS")
#define STR_SEARCHENGINE          QStringLiteral("SEARCHENGINE")
#define STR_HAVE_DOT              QStringLiteral("HAVE_DOT")
#define STR_CLASS_DIAGRAMS        QStringLiteral("CLASS_DIAGRAMS")
#define STR_CLASS_GRAPH           QStringLiteral("CLASS_GRAPH")
#define STR_COLLABORATION_GRAPH   QStringLiteral("COLLABORATION_GRAPH")
#define STR_GRAPHICAL_HIERARCHY   QStringLiteral("GRAPHICAL_HIERARCHY")
#define STR_INCLUDE_GRAPH         QStringLiteral("INCLUDE_GRAPH")
#define STR_INCLUDED_BY_GRAPH     QStringLiteral("INCLUDED_BY_GRAPH")
#define STR_CALL_GRAPH            QStringLiteral("CALL_GRAPH")
#define STR_CALLER_GRAPH          QStringLiteral("CALLER_GRAPH")
#define STR_HTML_COLORSTYLE_HUE   QStringLiteral("HTML_COLORSTYLE_HUE")
#define STR_HTML_COLORSTYLE_SAT   QStringLiteral("HTML_COLORSTYLE_SAT")
#define STR_HTML_COLORSTYLE_GAMMA QStringLiteral("HTML_COLORSTYLE_GAMMA")

static bool g_optimizeMapping[6][6] = 
{
  // A: OPTIMIZE_OUTPUT_FOR_C
  // B: OPTIMIZE_OUTPUT_JAVA
  // C: OPTIMIZE_FOR_FORTRAN
  // D: OPTIMIZE_OUTPUT_VHDL
  // E: CPP_CLI_SUPPORT
  // F: HIDE_SCOPE_NAMES
  // A     B     C     D     E      F
  { false,false,false,false,false,false }, // 0: C++
  { false,false,false,false,true, false }, // 1: C++/CLI
  { false,true, false,false,false,false }, // 2: C#/Java
  { true, false,false,false,false,true  }, // 3: C/PHP 
  { false,false,true, false,false,false }, // 4: Fortran
  { false,false,false,true, false,false }, // 5: VHDL
};

static QString g_optimizeOptionNames[6] =
{
  STR_OPTIMIZE_OUTPUT_FOR_C,
  STR_OPTIMIZE_OUTPUT_JAVA,
  STR_OPTIMIZE_FOR_FORTRAN,
  STR_OPTIMIZE_OUTPUT_VHDL,
  STR_CPP_CLI_SUPPORT,
  STR_HIDE_SCOPE_NAMES
};

//==========================================================================

static bool stringVariantToBool(const QVariant &v)
{
  QString s = v.toString().toLower();
  return s==QStringLiteral("yes") || s==QStringLiteral("true") || s==QStringLiteral("1");
} 

static bool getBoolOption(
    const QHash<QString,Input*>&model,const QString &name)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  return stringVariantToBool(option->value());
} 

static int getIntOption(
    const QHash<QString,Input*>&model,const QString &name)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  return option->value().toInt();
} 

static QString getStringOption(
    const QHash<QString,Input*>&model,const QString &name)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  return option->value().toString();
}

static void updateBoolOption(
    const QHash<QString,Input*>&model,const QString &name,bool bNew)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  bool bOld = stringVariantToBool(option->value());
  if (bOld!=bNew)
  {
    option->value()=(bNew ? QStringLiteral("true") : QStringLiteral("false"));
    option->update();
  }
}

static void updateIntOption(
    const QHash<QString,Input*>&model,const QString &name,int iNew)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  int iOld = option->value().toInt();
  if (iOld!=iNew)
  {
    option->value()=QStringLiteral("%1").arg(iNew);
    option->update();
  }
}


static void updateStringOption(
    const QHash<QString,Input*>&model,const QString &name,const QString &s)
{
  Input *option = model[name];
  Q_ASSERT(option!=0);
  if (option->value().toString()!=s)
  {
    option->value() = s;
    option->update();
  }
}

//==========================================================================

TuneColorDialog::TuneColorDialog(int hue,int sat,int gamma,QWidget *parent) : QDialog(parent)
{
   setWindowTitle(tr("Tune the color of the HTML output"));
   QGridLayout *layout = new QGridLayout(this);
   m_image = new QImage(QStringLiteral(":/images/tunecolor.png"));
   m_imageLab = new QLabel;
   updateImage(hue,sat,gamma);
   layout->addWidget(new QLabel(tr("Example output: use the sliders on the right to adjust the color")),0,0);
   layout->addWidget(m_imageLab,1,0);
   QHBoxLayout *buttonsLayout = new QHBoxLayout;

   QPushButton *okButton = new QPushButton(tr("Ok"));
   connect(okButton,SIGNAL(clicked()),SLOT(accept()));
   okButton->setDefault(true);
   QPushButton *cancelButton = new QPushButton(tr("Cancel"));
   connect(cancelButton,SIGNAL(clicked()),SLOT(reject()));

   ColorPicker *huePicker = new ColorPicker(ColorPicker::Hue);
   huePicker->setCol(hue,sat,gamma);
   huePicker->setFixedWidth(20);
   layout->addWidget(huePicker,1,1);
   ColorPicker *satPicker = new ColorPicker(ColorPicker::Saturation);
   satPicker->setCol(hue,sat,gamma);
   satPicker->setFixedWidth(20);
   layout->addWidget(satPicker,1,2);
   ColorPicker *gamPicker = new ColorPicker(ColorPicker::Gamma);
   gamPicker->setCol(hue,sat,gamma);
   gamPicker->setFixedWidth(20);
   layout->addWidget(gamPicker,1,3);

   connect(huePicker,SIGNAL(newHsv(int,int,int)),satPicker,SLOT(setCol(int,int,int)));
   connect(satPicker,SIGNAL(newHsv(int,int,int)),huePicker,SLOT(setCol(int,int,int)));
   connect(huePicker,SIGNAL(newHsv(int,int,int)),gamPicker,SLOT(setCol(int,int,int)));
   connect(satPicker,SIGNAL(newHsv(int,int,int)),gamPicker,SLOT(setCol(int,int,int)));
   connect(gamPicker,SIGNAL(newHsv(int,int,int)),satPicker,SLOT(setCol(int,int,int)));
   connect(gamPicker,SIGNAL(newHsv(int,int,int)),huePicker,SLOT(setCol(int,int,int)));
   connect(huePicker,SIGNAL(newHsv(int,int,int)),this,SLOT(updateImage(int,int,int)));
   connect(satPicker,SIGNAL(newHsv(int,int,int)),this,SLOT(updateImage(int,int,int)));
   connect(gamPicker,SIGNAL(newHsv(int,int,int)),this,SLOT(updateImage(int,int,int)));

   buttonsLayout->addStretch();
   buttonsLayout->addWidget(okButton);
   buttonsLayout->addWidget(cancelButton);
   layout->addLayout(buttonsLayout,5,0,1,4);
}

void hsl2rgb(double h,double s,double l,
             double *pRed,double *pGreen,double *pBlue)
{
  double v;
  double r,g,b;

  r = l;   // default to gray
  g = l;
  b = l;
  v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
  if (v > 0)
  {
    double m;
    double sv;
    int sextant;
    double fract, vsf, mid1, mid2;

    m       = l + l - v;
    sv      = (v - m ) / v;
    h      *= 6.0;
    sextant = (int)h;
    fract   = h - sextant;
    vsf     = v * sv * fract;
    mid1    = m + vsf;
    mid2    = v - vsf;
    switch (sextant)
    {
      case 0:
        r = v;
        g = mid1;
        b = m;
        break;
      case 1:
        r = mid2;
        g = v;
        b = m;
        break;
      case 2:
        r = m;
        g = v;
        b = mid1;
        break;
      case 3:
        r = m;
        g = mid2;
        b = v;
        break;
      case 4:
        r = mid1;
        g = m;
        b = v;
        break;
      case 5:
        r = v;
        g = m;
        b = mid2;
        break;
    }
  }
  *pRed   = r;
  *pGreen = g;
  *pBlue  = b;
}



void TuneColorDialog::updateImage(int hue,int sat,int gam)
{
  QImage coloredImg(m_image->width(),m_image->height(),QImage::Format_RGB32);
  uint *srcPixel = (uint *)m_image->scanLine(0);
  uint *dstPixel = (uint *)coloredImg.scanLine(0);
  uint nrPixels = coloredImg.width()*coloredImg.height();
  for (uint i=0;i<nrPixels;i++,srcPixel++,dstPixel++)
  {
    QColor c = QColor::fromRgb(*srcPixel);
    double r,g,b;
    hsl2rgb(hue/359.0, sat/255.0, pow(c.green()/255.0,gam/100.0),&r,&g,&b);
    *dstPixel = qRgb((int)(r*255.0),(int)(g*255.0),(int)(b*255.0));
  }
  m_imageLab->setPixmap(QPixmap::fromImage(coloredImg));
  m_hue = hue;
  m_sat = sat;
  m_gam = gam;
}

int TuneColorDialog::getHue() const
{
  return m_hue;
}

int TuneColorDialog::getSaturation() const
{
  return m_sat;
}

int TuneColorDialog::getGamma() const
{
  return m_gam;
}

//==========================================================================

ColorPicker::ColorPicker(Mode m)
{
  m_hue = 220;
  m_gam = 100;
  m_sat = 100;
  m_mode = m;
  m_pix = 0;
}

ColorPicker::~ColorPicker()
{
  delete m_pix;
}

void ColorPicker::paintEvent(QPaintEvent*)
{
  int w = width() - 5;

  QRect r(0, foff, w, height() - 2*foff);
  int wi = r.width() - 2;
  int hi = r.height() - 2;
  if (!m_pix || m_pix->height() != hi || m_pix->width() != wi) 
  {
    delete m_pix;
    QImage img(wi, hi, QImage::Format_RGB32);
    int y;
    uint *pixel = (uint *) img.scanLine(0);
    for (y = 0; y < hi; y++) 
    {
      const uint *end = pixel + wi;
      int yh = y2hue(y+coff);
      int ys = y2sat(y+coff);
      int yg = y2gam(y+coff);
      while (pixel < end) 
      {
        QColor c;
        c.setHsv(yh, ys, (int)(255*pow(0.7,yg/100.0)));
        *pixel = c.rgb();
        ++pixel;
      }
    }
    m_pix = new QPixmap(QPixmap::fromImage(img));
  }
  QPainter p(this);
  p.drawPixmap(1, coff, *m_pix);
  const QPalette &g = palette();
  qDrawShadePanel(&p, r, g, true);
  p.setPen(g.foreground().color());
  p.setBrush(g.foreground());
  QPolygon a;
  int y = m_mode==Hue ?        hue2y(m_hue) : 
          m_mode==Saturation ? sat2y(m_sat) :
                               gam2y(m_gam);
  a.setPoints(3, w, y, w+5, y+5, w+5, y-5);
  p.eraseRect(w, 0, 5, height());
  p.drawPolygon(a);
}

void ColorPicker::mouseMoveEvent(QMouseEvent *m)
{
  if      (m_mode==Hue)        setHue(y2hue(m->y())); 
  else if (m_mode==Saturation) setSat(y2sat(m->y()));
  else                         setGam(y2gam(m->y()));
}

void ColorPicker::mousePressEvent(QMouseEvent *m)
{
  if      (m_mode==Hue)        setHue(y2hue(m->y())); 
  else if (m_mode==Saturation) setSat(y2sat(m->y()));
  else                         setGam(y2gam(m->y()));
}

void ColorPicker::setHue(int h)
{
  if (h==m_hue) return;
  m_hue = qMax(0,qMin(h,359));
  delete m_pix; m_pix=0;
  repaint();
  emit newHsv(m_hue,m_sat,m_gam);
}

void ColorPicker::setSat(int s)
{
  if (s==m_sat) return;
  m_sat = qMax(0,qMin(s,255));
  delete m_pix; m_pix=0;
  repaint();
  emit newHsv(m_hue,m_sat,m_gam);
}

void ColorPicker::setGam(int g)
{
  if (g==m_gam) return;
  m_gam = qMax(40,qMin(g,240));
  delete m_pix; m_pix=0;
  repaint();
  emit newHsv(m_hue,m_sat,m_gam);
}

void ColorPicker::setCol(int h, int s, int g)
{
  if (m_hue!=h || m_sat!=s || m_gam!=g)
  {
    m_hue = h;
    m_sat = s;
    m_gam = g;
    delete m_pix; m_pix=0;
    repaint();
  }
}

int ColorPicker::y2hue(int y)
{
  int d = height() - 2*coff - 1;
  return m_mode==Hue ? (y - coff)*359/d : m_hue;
}

int ColorPicker::hue2y(int v)
{
  int d = height() - 2*coff - 1;
  return coff + v*d/359;
}

int ColorPicker::y2sat(int y)
{
  int d = height() - 2*coff - 1;
  return m_mode==Saturation ? 255 - (y - coff)*255/d : m_sat;
}

int ColorPicker::sat2y(int v)
{
  int d = height() - 2*coff - 1;
  return coff + (255-v)*d/255;
}

int ColorPicker::y2gam(int y)
{
  int d = height() - 2*coff - 1;
  return m_mode==Gamma ? 240 - (y - coff)*200/d : m_gam;
}

int ColorPicker::gam2y(int g)
{
  int d = height() - 2*coff - 1;
  return coff + (240-g)*d/200;
}

//==========================================================================

Step1::Step1(Wizard *wizard,const QHash<QString,Input*> &modelData) : m_wizard(wizard), m_modelData(modelData)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(4);
  layout->setSpacing(8);
  QLabel *l = new QLabel(this);
  l->setText(tr("Provide some information "
              "about the project you are documenting"));
  layout->addWidget(l);
  QWidget *w      = new QWidget( this );
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(10);

  // project name
  QLabel *projName = new QLabel(this);
  projName->setText(tr("Project name:"));
  projName->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  // project brief
  QLabel *projBrief = new QLabel(this);
  projBrief->setText(tr("Project synopsis:"));
  projBrief->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  // project version
  QLabel *projVersion = new QLabel(this);
  projVersion->setText(tr("Project version or id:"));
  projVersion->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  // project icon
  QLabel *projLogo = new QLabel(this);
  projLogo->setMinimumSize(1,55);
  projLogo->setText(tr("Project logo:"));
  projLogo->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  grid->addWidget(projName,0,0);
  grid->addWidget(projBrief,1,0);
  grid->addWidget(projVersion,2,0);
  grid->addWidget(projLogo,3,0);

  m_projName   = new QLineEdit;
  m_projBrief  = new QLineEdit;
  m_projNumber = new QLineEdit;
  QPushButton *projIconSel = new QPushButton(this);
  projIconSel->setText(tr("Select..."));
  m_projIconLab = new QLabel;

  grid->addWidget(m_projName,0,1,1,2);
  grid->addWidget(m_projBrief,1,1,1,2);
  grid->addWidget(m_projNumber,2,1,1,2);
  grid->addWidget(projIconSel,3,1);
  grid->addWidget(m_projIconLab,3,2);

  grid->setColumnStretch(2,1);

  w->setLayout(grid);

  layout->addWidget(w);

  //---------------------------------------------------
  QFrame *f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);
  
  l = new QLabel(this);
  l->setText(tr("Specify the directory to scan for source code"));
  layout->addWidget(l);
  QWidget *row = new QWidget;
  QHBoxLayout *rowLayout = new QHBoxLayout(row);
  rowLayout->setSpacing(10);
  l = new QLabel(this);
  l->setText(tr("Source code directory:"));
  rowLayout->addWidget(l);
  m_sourceDir = new QLineEdit;
  m_srcSelectDir = new QPushButton(this);
  m_srcSelectDir->setText(tr("Select..."));
  rowLayout->addWidget(m_sourceDir);
  rowLayout->addWidget(m_srcSelectDir);
  layout->addWidget(row);

  m_recursive = new QCheckBox(this);
  m_recursive->setText(tr("Scan recursively"));
  m_recursive->setChecked(true);
  layout->addWidget(m_recursive);

  //---------------------------------------------------
  f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);

  l = new QLabel(this);
  l->setText(tr("Specify the directory where doxygen should "
              "put the generated documentation"));
  layout->addWidget(l);
  row = new QWidget;
  rowLayout = new QHBoxLayout(row);
  rowLayout->setSpacing(10);
  l = new QLabel(this);
  l->setText(tr("Destination directory:"));
  rowLayout->addWidget(l);
  m_destDir = new QLineEdit;
  m_dstSelectDir = new QPushButton(this);
  m_dstSelectDir->setText(tr("Select..."));
  rowLayout->addWidget(m_destDir);
  rowLayout->addWidget(m_dstSelectDir);
  layout->addWidget(row);
  layout->addStretch(1);
  setLayout(layout);

  connect(projIconSel,SIGNAL(clicked()),
          this,SLOT(selectProjectIcon()));
  connect(m_srcSelectDir,SIGNAL(clicked()),
          this,SLOT(selectSourceDir()));
  connect(m_dstSelectDir,SIGNAL(clicked()),
          this,SLOT(selectDestinationDir()));
  connect(m_projName,SIGNAL(textChanged(const QString &)),SLOT(setProjectName(const QString &)));
  connect(m_projBrief,SIGNAL(textChanged(const QString &)),SLOT(setProjectBrief(const QString &)));
  connect(m_projNumber,SIGNAL(textChanged(const QString &)),SLOT(setProjectNumber(const QString &)));
  connect(m_sourceDir,SIGNAL(textChanged(const QString &)),SLOT(setSourceDir(const QString &)));
  connect(m_recursive,SIGNAL(stateChanged(int)),SLOT(setRecursiveScan(int)));
  connect(m_destDir,SIGNAL(textChanged(const QString &)),SLOT(setDestinationDir(const QString &)));
}

void Step1::selectProjectIcon()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  QString iconName = QFileDialog::getOpenFileName(this,
                                    tr("Select project icon/image"),path);
  if (iconName.isEmpty())
  {
    m_projIconLab->setText(tr("No Project logo selected."));
  }
  else
  {
    QFile Fout(iconName);
    if(!Fout.exists()) 
    {
      m_projIconLab->setText(tr("Sorry, cannot find file(")+iconName+QStringLiteral(");"));
    }
    else
    {
      QPixmap pm(iconName);
      if (!pm.isNull())
      {
        m_projIconLab->setPixmap(pm.scaledToHeight(55,Qt::SmoothTransformation));
      }
      else
      {
        m_projIconLab->setText(tr("Sorry, no preview available (")+iconName+QStringLiteral(");"));
      }
    }
  }
  updateStringOption(m_modelData,STR_PROJECT_LOGO,iconName);
}

void Step1::selectSourceDir()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Select source directory"),path);
  QDir dir(path);
  if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
  {
    dirName = dir.relativeFilePath(dirName);
  }
  if (dirName.isEmpty())
  {
    dirName=QStringLiteral(".");
  }
  m_sourceDir->setText(dirName);
}

void Step1::selectDestinationDir()
{
  QString path = QFileInfo(MainWindow::instance().configFileName()).path();
  QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Select destination directory"),path);
  QDir dir(path);
  if (!MainWindow::instance().configFileName().isEmpty() && dir.exists())
  {
    dirName = dir.relativeFilePath(dirName);
  }
  if (dirName.isEmpty())
  {
    dirName=QStringLiteral(".");
  }
  m_destDir->setText(dirName);
}

void Step1::setProjectName(const QString &name)
{
  updateStringOption(m_modelData,STR_PROJECT_NAME,name);
}

void Step1::setProjectBrief(const QString &desc)
{
  updateStringOption(m_modelData,STR_PROJECT_BRIEF,desc);
}

void Step1::setProjectNumber(const QString &num)
{
  updateStringOption(m_modelData,STR_PROJECT_NUMBER,num);
}

void Step1::setSourceDir(const QString &dir)
{
  Input *option = m_modelData[STR_INPUT];
  if (option->value().toStringList().count()>0)
  {
    QStringList sl = option->value().toStringList();
    if (sl[0]!=dir)
    {
      sl[0] = dir;
      option->value() = sl;
      option->update();
    }
  }
  else
  {
    option->value() = QStringList() << dir;
    option->update();
  }
}

void Step1::setDestinationDir(const QString &dir)
{
  updateStringOption(m_modelData,STR_OUTPUT_DIRECTORY,dir);
}

void Step1::setRecursiveScan(int s)
{
  updateBoolOption(m_modelData,STR_RECURSIVE,s==Qt::Checked);
}

void Step1::init()
{
  Input *option;
  m_projName->setText(getStringOption(m_modelData,STR_PROJECT_NAME));
  m_projBrief->setText(getStringOption(m_modelData,STR_PROJECT_BRIEF));
  m_projNumber->setText(getStringOption(m_modelData,STR_PROJECT_NUMBER));
  QString iconName = getStringOption(m_modelData,STR_PROJECT_LOGO);
  if (!iconName.isEmpty())
  {
    QFile Fout(iconName);
    if(!Fout.exists()) 
    {
      m_projIconLab->setText(tr("Sorry, cannot find file(")+iconName+QStringLiteral(");"));
    }
    else
    {
      QPixmap pm(iconName);
      if (!pm.isNull())
      {
        m_projIconLab->setPixmap(pm.scaledToHeight(55,Qt::SmoothTransformation));
      }
      else
      {
        m_projIconLab->setText(tr("Sorry, no preview available (")+iconName+QStringLiteral(");"));
      }
    }
  }
  else
  {
    m_projIconLab->setText(tr("No Project logo selected."));
  }
  option = m_modelData[STR_INPUT];
  if (option->value().toStringList().count()>0)
  {
    m_sourceDir->setText(option->value().toStringList().first());
  }
  m_recursive->setChecked(
      getBoolOption(m_modelData,STR_RECURSIVE) ? Qt::Checked : Qt::Unchecked);
  m_destDir->setText(getStringOption(m_modelData,STR_OUTPUT_DIRECTORY));
}


//==========================================================================

Step2::Step2(Wizard *wizard,const QHash<QString,Input*> &modelData) 
  : m_wizard(wizard), m_modelData(modelData)
{
  QRadioButton *r;
  QVBoxLayout *layout = new QVBoxLayout(this);

  //---------------------------------------------------
  m_extractModeGroup = new QButtonGroup(this);
  m_extractMode = new QGroupBox(this);
  m_extractMode->setTitle(tr("Select the desired extraction mode:"));
  QGridLayout *gbox = new QGridLayout( m_extractMode );
  r = new QRadioButton(tr("Documented entities only"));
  r->setChecked(true);
  m_extractModeGroup->addButton(r, 0);
  gbox->addWidget(r,1,0);
  // 1 -> EXTRACT_ALL = NO
  r = new QRadioButton(tr("All Entities"));
  m_extractModeGroup->addButton(r, 1);
  gbox->addWidget(r,2,0);
  // 2 -> EXTRACT_ALL = YES
  m_crossRef = new QCheckBox(m_extractMode);
  m_crossRef->setText(tr("Include cross-referenced source code in the output"));
  // m_crossRef -> SOURCE_BROWSER = YES/NO
  gbox->addWidget(m_crossRef,3,0);
  layout->addWidget(m_extractMode);
  
  //---------------------------------------------------
  QFrame *f = new QFrame( this );
  f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget(f);

  m_optimizeLangGroup = new QButtonGroup(this);
  m_optimizeLang = new QGroupBox(this);
  m_optimizeLang->setTitle(tr("Select programming language to optimize the results for"));
  gbox = new QGridLayout( m_optimizeLang ); 
  
  r = new QRadioButton(m_optimizeLang);
  r->setText(tr("Optimize for C++ output"));
  r->setChecked(true);
  m_optimizeLangGroup->addButton(r, 0);
  // 0 -> OPTIMIZE_OUTPUT_FOR_C = NO
  //      OPTIMIZE_OUTPUT_JAVA  = NO
  //      OPTIMIZE_FOR_FORTRAN  = NO
  //      OPTIMIZE_OUTPUT_VHDL  = NO
  //      CPP_CLI_SUPPORT       = NO
  //      HIDE_SCOPE_NAMES      = NO
  gbox->addWidget(r,0,0);
  r = new QRadioButton(tr("Optimize for C++/CLI output"));
  gbox->addWidget(r,1,0);
  m_optimizeLangGroup->addButton(r, 1);
  // 1 -> OPTIMIZE_OUTPUT_FOR_C = NO
  //      OPTIMIZE_OUTPUT_JAVA  = NO
  //      OPTIMIZE_FOR_FORTRAN  = NO
  //      OPTIMIZE_OUTPUT_VHDL  = NO
  //      CPP_CLI_SUPPORT       = YES
  //      HIDE_SCOPE_NAMES      = NO
  r = new QRadioButton(tr("Optimize for Java or C# output"));
  m_optimizeLangGroup->addButton(r, 2);
  // 2 -> OPTIMIZE_OUTPUT_FOR_C = NO
  //      OPTIMIZE_OUTPUT_JAVA  = YES
  //      OPTIMIZE_FOR_FORTRAN  = NO
  //      OPTIMIZE_OUTPUT_VHDL  = NO
  //      CPP_CLI_SUPPORT       = NO
  //      HIDE_SCOPE_NAMES      = NO
  gbox->addWidget(r,2,0);
  r = new QRadioButton(tr("Optimize for C or PHP output"));
  m_optimizeLangGroup->addButton(r, 3);
  // 3 -> OPTIMIZE_OUTPUT_FOR_C = YES
  //      OPTIMIZE_OUTPUT_JAVA  = NO
  //      OPTIMIZE_FOR_FORTRAN  = NO
  //      OPTIMIZE_OUTPUT_VHDL  = NO
  //      CPP_CLI_SUPPORT       = NO
  //      HIDE_SCOPE_NAMES      = YES
  gbox->addWidget(r,3,0);
  r = new QRadioButton(tr("Optimize for Fortran output"));
  m_optimizeLangGroup->addButton(r, 4);
  // 4 -> OPTIMIZE_OUTPUT_FOR_C = NO
  //      OPTIMIZE_OUTPUT_JAVA  = NO
  //      OPTIMIZE_FOR_FORTRAN  = YES
  //      OPTIMIZE_OUTPUT_VHDL  = NO
  //      CPP_CLI_SUPPORT       = NO
  //      HIDE_SCOPE_NAMES      = NO
  gbox->addWidget(r,4,0);
  r = new QRadioButton(tr("Optimize for VHDL output"));
  m_optimizeLangGroup->addButton(r, 5);
  // 5 -> OPTIMIZE_OUTPUT_FOR_C = NO
  //      OPTIMIZE_OUTPUT_JAVA  = NO
  //      OPTIMIZE_FOR_FORTRAN  = NO
  //      OPTIMIZE_OUTPUT_VHDL  = YES
  //      CPP_CLI_SUPPORT       = NO
  //      HIDE_SCOPE_NAMES      = NO
  gbox->addWidget(r,5,0);

  layout->addWidget(m_optimizeLang);
  layout->addStretch(1);

  connect(m_crossRef,SIGNAL(stateChanged(int)),
          SLOT(changeCrossRefState(int)));
  connect(m_optimizeLangGroup,SIGNAL(buttonClicked(int)),
          SLOT(optimizeFor(int)));
  connect(m_extractModeGroup,SIGNAL(buttonClicked(int)),
          SLOT(extractMode(int)));
}


void Step2::optimizeFor(int choice)
{
  for (int i=0;i<6;i++)
  {
    updateBoolOption(m_modelData,
                     g_optimizeOptionNames[i],
                     g_optimizeMapping[choice][i]);
  }
}

void Step2::extractMode(int choice)
{
  updateBoolOption(m_modelData,STR_EXTRACT_ALL,choice==1);
}

void Step2::changeCrossRefState(int choice)
{
  updateBoolOption(m_modelData,STR_SOURCE_BROWSER,choice==Qt::Checked);
}

void Step2::init()
{
  m_extractModeGroup->button(
      getBoolOption(m_modelData,STR_EXTRACT_ALL) ? 1 : 0)->setChecked(true);
  m_crossRef->setChecked(getBoolOption(m_modelData,STR_SOURCE_BROWSER));

  int x=0;
  if (getBoolOption(m_modelData,STR_CPP_CLI_SUPPORT))            x=1;
  else if (getBoolOption(m_modelData,STR_OPTIMIZE_OUTPUT_JAVA))  x=2;
  else if (getBoolOption(m_modelData,STR_OPTIMIZE_OUTPUT_FOR_C)) x=3;
  else if (getBoolOption(m_modelData,STR_OPTIMIZE_FOR_FORTRAN))  x=4;
  else if (getBoolOption(m_modelData,STR_OPTIMIZE_OUTPUT_VHDL))  x=5;
  m_optimizeLangGroup->button(x)->setChecked(true);
}

//==========================================================================

Step3::Step3(Wizard *wizard,const QHash<QString,Input*> &modelData) 
  : m_wizard(wizard), m_modelData(modelData)
{
  QVBoxLayout *vbox = 0;
  QRadioButton *r   = 0;

  QGridLayout *gbox = new QGridLayout( this );
  gbox->addWidget(new QLabel(tr("Select the output format(s) to generate")),0,0);
  {
    m_htmlOptions = new QGroupBox(tr("HTML"));
    m_htmlOptions->setCheckable(true);
    // GENERATE_HTML
    m_htmlOptionsGroup = new QButtonGroup(m_htmlOptions);
    QRadioButton *r = new QRadioButton(tr("plain HTML"));
    r->setChecked(true);
    m_htmlOptionsGroup->addButton(r, 0);
    vbox = new QVBoxLayout;
    vbox->addWidget(r);
    r = new QRadioButton(tr("with navigation panel"));
    m_htmlOptionsGroup->addButton(r, 1);
    // GENERATE_TREEVIEW
    vbox->addWidget(r);
    r = new QRadioButton(tr("prepare for compressed HTML (.chm)"));
    m_htmlOptionsGroup->addButton(r, 2);
    // GENERATE_HTMLHELP
    vbox->addWidget(r);
    m_searchEnabled=new QCheckBox(tr("With search function"));
    vbox->addWidget(m_searchEnabled);
    // SEARCH_ENGINE
    QHBoxLayout *hbox = new QHBoxLayout;
    m_tuneColor=new QPushButton(tr("Change color..."));
    hbox->addWidget(m_tuneColor);
    hbox->addStretch(1);
    vbox->addLayout(hbox);
    m_htmlOptions->setLayout(vbox);
    m_htmlOptions->setChecked(true);
  }
  gbox->addWidget(m_htmlOptions,1,0);

  {
    m_texOptions = new QGroupBox(tr("LaTeX"));
    m_texOptions->setCheckable(true);
    // GENERATE_LATEX
    m_texOptionsGroup = new QButtonGroup(m_texOptions);
    vbox = new QVBoxLayout;
    r = new QRadioButton(tr("as intermediate format for hyperlinked PDF"));
    m_texOptionsGroup->addButton(r, 0);
    // PDF_HYPERLINKS = YES
    r->setChecked(true);
    vbox->addWidget(r);
    r = new QRadioButton(tr("as intermediate format for PDF"));
    m_texOptionsGroup->addButton(r, 1);
    // PDF_HYPERLINKS = NO, USE_PDFLATEX = YES
    vbox->addWidget(r);
    r = new QRadioButton(tr("as intermediate format for PostScript"));
    m_texOptionsGroup->addButton(r, 2);
    // USE_PDFLATEX = NO
    vbox->addWidget(r);
    vbox->addStretch(1);
    m_texOptions->setLayout(vbox);
    m_texOptions->setChecked(true);
  }
  gbox->addWidget(m_texOptions,2,0);

  m_manEnabled=new QCheckBox(tr("Man pages"));
  // GENERATE_MAN
  m_rtfEnabled=new QCheckBox(tr("Rich Text Format (RTF)"));
  // GENERATE_RTF
  m_xmlEnabled=new QCheckBox(tr("XML"));
  // GENERATE_XML
  gbox->addWidget(m_manEnabled,3,0);
  gbox->addWidget(m_rtfEnabled,4,0);
  gbox->addWidget(m_xmlEnabled,5,0);

  gbox->setRowStretch(6,1);
  connect(m_htmlOptions,SIGNAL(toggled(bool)),SLOT(setHtmlEnabled(bool)));
  connect(m_texOptions,SIGNAL(toggled(bool)),SLOT(setLatexEnabled(bool)));
  connect(m_manEnabled,SIGNAL(stateChanged(int)),SLOT(setManEnabled(int)));
  connect(m_rtfEnabled,SIGNAL(stateChanged(int)),SLOT(setRtfEnabled(int)));
  connect(m_xmlEnabled,SIGNAL(stateChanged(int)),SLOT(setXmlEnabled(int)));
  connect(m_searchEnabled,SIGNAL(stateChanged(int)),SLOT(setSearchEnabled(int)));
  connect(m_htmlOptionsGroup,SIGNAL(buttonClicked(int)),
          SLOT(setHtmlOptions(int)));
  connect(m_texOptionsGroup,SIGNAL(buttonClicked(int)),
          SLOT(setLatexOptions(int)));
  connect(m_tuneColor,SIGNAL(clicked()),SLOT(tuneColorDialog()));
}

void Step3::tuneColorDialog()
{
  int hue = getIntOption(m_modelData,STR_HTML_COLORSTYLE_HUE);
  int sat = getIntOption(m_modelData,STR_HTML_COLORSTYLE_SAT);
  int gam = getIntOption(m_modelData,STR_HTML_COLORSTYLE_GAMMA);
  TuneColorDialog tuneColor(hue,sat,gam,this);
  if (tuneColor.exec()==QDialog::Accepted)
  {
    updateIntOption(m_modelData,STR_HTML_COLORSTYLE_HUE,tuneColor.getHue());
    updateIntOption(m_modelData,STR_HTML_COLORSTYLE_SAT,tuneColor.getSaturation());
    updateIntOption(m_modelData,STR_HTML_COLORSTYLE_GAMMA,tuneColor.getGamma());
  }
}

void Step3::setHtmlEnabled(bool b)
{
  updateBoolOption(m_modelData,STR_GENERATE_HTML,b);
}

void Step3::setLatexEnabled(bool b)
{
  updateBoolOption(m_modelData,STR_GENERATE_LATEX,b);
}

void Step3::setManEnabled(int state)
{
  updateBoolOption(m_modelData,STR_GENERATE_MAN,state==Qt::Checked);
}

void Step3::setRtfEnabled(int state)
{
  updateBoolOption(m_modelData,STR_GENERATE_RTF,state==Qt::Checked);
}

void Step3::setXmlEnabled(int state)
{
  updateBoolOption(m_modelData,STR_GENERATE_XML,state==Qt::Checked);
}

void Step3::setSearchEnabled(int state)
{
  updateBoolOption(m_modelData,STR_SEARCHENGINE,state==Qt::Checked);
}

void Step3::setHtmlOptions(int id)
{
  if (id==0) // plain HTML
  {
    updateBoolOption(m_modelData,STR_GENERATE_HTMLHELP,false);
    updateBoolOption(m_modelData,STR_GENERATE_TREEVIEW,false);
  }
  else if (id==1) // with navigation tree
  {
    updateBoolOption(m_modelData,STR_GENERATE_HTMLHELP,false);
    updateBoolOption(m_modelData,STR_GENERATE_TREEVIEW,true);
  }
  else if (id==2) // with compiled help
  {
    updateBoolOption(m_modelData,STR_GENERATE_HTMLHELP,true);
    updateBoolOption(m_modelData,STR_GENERATE_TREEVIEW,false);
  }
}

void Step3::setLatexOptions(int id)
{
  if (id==0) // hyperlinked PDF
  {
    updateBoolOption(m_modelData,STR_USE_PDFLATEX,true);
    updateBoolOption(m_modelData,STR_PDF_HYPERLINKS,true);
  }
  else if (id==1) // PDF
  {
    updateBoolOption(m_modelData,STR_USE_PDFLATEX,true);
    updateBoolOption(m_modelData,STR_PDF_HYPERLINKS,false);
  }
  else if (id==2) // PostScript
  {
    updateBoolOption(m_modelData,STR_USE_PDFLATEX,false);
    updateBoolOption(m_modelData,STR_PDF_HYPERLINKS,false);
  }
}

void Step3::init()
{
  m_htmlOptions->setChecked(getBoolOption(m_modelData,STR_GENERATE_HTML));
  m_texOptions->setChecked(getBoolOption(m_modelData,STR_GENERATE_LATEX));
  m_manEnabled->setChecked(getBoolOption(m_modelData,STR_GENERATE_MAN));
  m_rtfEnabled->setChecked(getBoolOption(m_modelData,STR_GENERATE_RTF));
  m_xmlEnabled->setChecked(getBoolOption(m_modelData,STR_GENERATE_XML));
  m_searchEnabled->setChecked(getBoolOption(m_modelData,STR_SEARCHENGINE));
  if (getBoolOption(m_modelData,STR_GENERATE_HTMLHELP))
  {
    m_htmlOptionsGroup->button(2)->setChecked(true); // compiled help
  }
  else if (getBoolOption(m_modelData,STR_GENERATE_TREEVIEW))
  {
    m_htmlOptionsGroup->button(1)->setChecked(true); // navigation tree
  }
  else
  {
    m_htmlOptionsGroup->button(0)->setChecked(true); // plain HTML
  }
  if (!getBoolOption(m_modelData,STR_USE_PDFLATEX))
  {
    m_texOptionsGroup->button(2)->setChecked(true); // PostScript
  }
  else if (!getBoolOption(m_modelData,STR_PDF_HYPERLINKS))
  {
    m_texOptionsGroup->button(1)->setChecked(true); // Plain PDF
  }
  else
  {
    m_texOptionsGroup->button(0)->setChecked(true); // PDF with hyperlinks
  }
}

//==========================================================================

Step4::Step4(Wizard *wizard,const QHash<QString,Input*> &modelData) 
  : m_wizard(wizard), m_modelData(modelData)
{
  m_diagramModeGroup = new QButtonGroup(this);
  QGridLayout *gbox = new QGridLayout( this );
  gbox->addWidget(new QLabel(tr("Diagrams to generate")),0,0);

  QRadioButton *rb = new QRadioButton(tr("No diagrams"));
  m_diagramModeGroup->addButton(rb, 0);
  gbox->addWidget(rb,1,0);
  // CLASS_DIAGRAMS = NO, HAVE_DOT = NO
  rb->setChecked(true);
  rb = new QRadioButton(tr("Use built-in class diagram generator"));
  m_diagramModeGroup->addButton(rb, 1);
  // CLASS_DIAGRAMS = YES, HAVE_DOT = NO
  gbox->addWidget(rb,2,0);
  rb = new QRadioButton(tr("Use dot tool from the GraphViz package"));
  m_diagramModeGroup->addButton(rb, 2);
  gbox->addWidget(rb,3,0);
  // CLASS_DIAGRAMS = NO, HAVE_DOT = YES

  m_dotGroup = new QGroupBox(tr("Dot graphs to generate"));
    QVBoxLayout *vbox = new QVBoxLayout;
    m_dotClass=new QCheckBox(tr("Class diagrams"));
    // CLASS_GRAPH
    m_dotCollaboration=new QCheckBox(tr("Collaboration diagrams"));
    // COLLABORATION_GRAPH
    m_dotInheritance=new QCheckBox(tr("Overall Class hierarchy"));
    // GRAPHICAL_HIERARCHY
    m_dotInclude=new QCheckBox(tr("Include dependency graphs"));
    // INCLUDE_GRAPH
    m_dotIncludedBy=new QCheckBox(tr("Included by dependency graphs"));
    // INCLUDED_BY_GRAPH
    m_dotCall=new QCheckBox(tr("Call graphs"));
    // CALL_GRAPH
    m_dotCaller=new QCheckBox(tr("Called by graphs"));
    // CALLER_GRAPH
    vbox->addWidget(m_dotClass);
    vbox->addWidget(m_dotCollaboration);
    vbox->addWidget(m_dotInheritance);
    vbox->addWidget(m_dotInclude);
    vbox->addWidget(m_dotIncludedBy);
    vbox->addWidget(m_dotCall);
    vbox->addWidget(m_dotCaller);
    vbox->addStretch(1);
    m_dotGroup->setLayout(vbox);
    m_dotClass->setChecked(true);
    m_dotGroup->setEnabled(false);
  gbox->addWidget(m_dotGroup,4,0);

  m_dotInclude->setChecked(true);
  m_dotCollaboration->setChecked(true);
  gbox->setRowStretch(5,1);

  connect(m_diagramModeGroup,SIGNAL(buttonClicked(int)),
          this,SLOT(diagramModeChanged(int)));
  connect(m_dotClass,SIGNAL(stateChanged(int)),
          this,SLOT(setClassGraphEnabled(int)));
  connect(m_dotCollaboration,SIGNAL(stateChanged(int)),
          this,SLOT(setCollaborationGraphEnabled(int)));
  connect(m_dotInheritance,SIGNAL(stateChanged(int)),
          this,SLOT(setGraphicalHierarchyEnabled(int)));
  connect(m_dotInclude,SIGNAL(stateChanged(int)),
          this,SLOT(setIncludeGraphEnabled(int)));
  connect(m_dotIncludedBy,SIGNAL(stateChanged(int)),
          this,SLOT(setIncludedByGraphEnabled(int)));
  connect(m_dotCall,SIGNAL(stateChanged(int)),
          this,SLOT(setCallGraphEnabled(int)));
  connect(m_dotCaller,SIGNAL(stateChanged(int)),
          this,SLOT(setCallerGraphEnabled(int)));
}

void Step4::diagramModeChanged(int id)
{
  if (id==0) // no diagrams
  {
    updateBoolOption(m_modelData,STR_HAVE_DOT,false);
    updateBoolOption(m_modelData,STR_CLASS_DIAGRAMS,false);
  }
  else if (id==1) // builtin diagrams
  {
    updateBoolOption(m_modelData,STR_HAVE_DOT,false);
    updateBoolOption(m_modelData,STR_CLASS_DIAGRAMS,true);
  }
  else if (id==2) // dot diagrams
  {
    updateBoolOption(m_modelData,STR_HAVE_DOT,true);
    updateBoolOption(m_modelData,STR_CLASS_DIAGRAMS,false);
  }
  m_dotGroup->setEnabled(id==2);
}

void Step4::setClassGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_CLASS_GRAPH,state==Qt::Checked);
}

void Step4::setCollaborationGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_COLLABORATION_GRAPH,state==Qt::Checked);
}

void Step4::setGraphicalHierarchyEnabled(int state)
{
  updateBoolOption(m_modelData,STR_GRAPHICAL_HIERARCHY,state==Qt::Checked);
}

void Step4::setIncludeGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_INCLUDE_GRAPH,state==Qt::Checked);
}

void Step4::setIncludedByGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_INCLUDED_BY_GRAPH,state==Qt::Checked);
}

void Step4::setCallGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_CALL_GRAPH,state==Qt::Checked);
}

void Step4::setCallerGraphEnabled(int state)
{
  updateBoolOption(m_modelData,STR_CALLER_GRAPH,state==Qt::Checked);
}

void Step4::init()
{
  if (getBoolOption(m_modelData,STR_HAVE_DOT))
  {
    m_diagramModeGroup->button(2)->setChecked(true); // Dot
  }
  else if (getBoolOption(m_modelData,STR_CLASS_DIAGRAMS))
  {
    m_diagramModeGroup->button(1)->setChecked(true); // Builtin diagrams
  }
  else
  {
    m_diagramModeGroup->button(0)->setChecked(true); // no diagrams
  }
  m_dotClass->setChecked(getBoolOption(m_modelData,STR_CLASS_GRAPH));
  m_dotCollaboration->setChecked(getBoolOption(m_modelData,STR_COLLABORATION_GRAPH));
  m_dotInheritance->setChecked(getBoolOption(m_modelData,STR_GRAPHICAL_HIERARCHY));
  m_dotInclude->setChecked(getBoolOption(m_modelData,STR_INCLUDE_GRAPH));
  m_dotIncludedBy->setChecked(getBoolOption(m_modelData,STR_INCLUDED_BY_GRAPH));
  m_dotCall->setChecked(getBoolOption(m_modelData,STR_CALL_GRAPH));
  m_dotCaller->setChecked(getBoolOption(m_modelData,STR_CALLER_GRAPH));
}

//==========================================================================

Wizard::Wizard(const QHash<QString,Input*> &modelData, QWidget *parent) : 
  QSplitter(parent), m_modelData(modelData)
{
  m_treeWidget = new QTreeWidget;
  m_treeWidget->setColumnCount(1);
  m_treeWidget->setHeaderLabels(QStringList() << QStringLiteral("Topics"));
  QList<QTreeWidgetItem*> items;
  items.append(new QTreeWidgetItem((QTreeWidget*)0,QStringList(tr("Project"))));
  items.append(new QTreeWidgetItem((QTreeWidget*)0,QStringList(tr("Mode"))));
  items.append(new QTreeWidgetItem((QTreeWidget*)0,QStringList(tr("Output"))));
  items.append(new QTreeWidgetItem((QTreeWidget*)0,QStringList(tr("Diagrams"))));
  m_treeWidget->insertTopLevelItems(0,items);

  m_topicStack = new QStackedWidget;
  m_step1 = new Step1(this,modelData);
  m_step2 = new Step2(this,modelData);
  m_step3 = new Step3(this,modelData);
  m_step4 = new Step4(this,modelData);
  m_topicStack->addWidget(m_step1);
  m_topicStack->addWidget(m_step2);
  m_topicStack->addWidget(m_step3);
  m_topicStack->addWidget(m_step4);

  QWidget *rightSide = new QWidget;
  QGridLayout *grid = new QGridLayout(rightSide);
  m_prev = new QPushButton(tr("Previous"));
  m_prev->setEnabled(false);
  m_next = new QPushButton(tr("Next"));
  grid->addWidget(m_topicStack,0,0,1,2);
  grid->addWidget(m_prev,1,0,Qt::AlignLeft);
  grid->addWidget(m_next,1,1,Qt::AlignRight);
  grid->setColumnStretch(0,1);
  grid->setRowStretch(0,1);
  addWidget(m_treeWidget);
  addWidget(rightSide);

  connect(m_treeWidget,
          SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
          SLOT(activateTopic(QTreeWidgetItem *,QTreeWidgetItem *)));
  connect(m_next,SIGNAL(clicked()),SLOT(nextTopic()));
  connect(m_prev,SIGNAL(clicked()),SLOT(prevTopic()));

  refresh();
}

Wizard::~Wizard()
{
}

void Wizard::activateTopic(QTreeWidgetItem *item,QTreeWidgetItem *)
{
  if (item)
  {
    
    QString label = item->text(0);
    if (label==tr("Project"))
    {
      m_topicStack->setCurrentWidget(m_step1);
      m_prev->setEnabled(false);
      m_next->setEnabled(true);
    }
    else if (label==tr("Mode"))
    {
      m_topicStack->setCurrentWidget(m_step2);
      m_prev->setEnabled(true);
      m_next->setEnabled(true);
    }
    else if (label==tr("Output"))
    {
      m_topicStack->setCurrentWidget(m_step3);
      m_prev->setEnabled(true);
      m_next->setEnabled(true);
    }
    else if (label==tr("Diagrams"))
    {
      m_topicStack->setCurrentWidget(m_step4);
      m_prev->setEnabled(true);
      m_next->setEnabled(true);
    }
  }
}

void Wizard::nextTopic()
{
  if (m_topicStack->currentIndex()+1==m_topicStack->count()) // last topic
  {
    done();
  }
  else
  {
    m_topicStack->setCurrentIndex(m_topicStack->currentIndex()+1);
    m_next->setEnabled(m_topicStack->count()!=m_topicStack->currentIndex()+1);
    m_prev->setEnabled(m_topicStack->currentIndex()!=0);
    m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(m_topicStack->currentIndex()));
  }
}

void Wizard::prevTopic()
{
  m_topicStack->setCurrentIndex(m_topicStack->currentIndex()-1);
  m_next->setEnabled(m_topicStack->count()!=m_topicStack->currentIndex()+1);
  m_prev->setEnabled(m_topicStack->currentIndex()!=0);
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(m_topicStack->currentIndex()));
}

void Wizard::refresh()
{
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(0));
  m_step1->init();
  m_step2->init();
  m_step3->init();
  m_step4->init();
}
