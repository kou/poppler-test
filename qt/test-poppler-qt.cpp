#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <ctype.h>
#include <poppler-qt.h>

class PDFDisplay : public QWidget           // picture display widget
{
public:
    PDFDisplay( Poppler::Document *d );
   ~PDFDisplay();
protected:
    void        paintEvent( QPaintEvent * );
private:
    QPixmap	*pixmap;
    Poppler::Document *doc;
};

PDFDisplay::PDFDisplay( Poppler::Document *d )
{
  doc = d;
  if (doc) {
    Poppler::Page *page = doc->getPage(0);
    if (page) {
      page->renderToPixmap(&pixmap, -1, -1, -1, -1);
      delete page;
    }
  } else {
    printf("doc not loaded\n");
  }
}

PDFDisplay::~PDFDisplay()
{
  delete doc;
  delete pixmap;
}

void PDFDisplay::paintEvent( QPaintEvent *e )
{
  QPainter paint( this );                     // paint widget
  if (pixmap)
    paint.drawPixmap(0, 0, *pixmap);
}

int main( int argc, char **argv )
{
  QApplication a( argc, argv );               // QApplication required!

  if ( argc < 2  || (argc == 3 && strcmp(argv[2], "-extract") != 0) || argc > 3)
  {
    // use argument as file name
    printf("usage: test-poppler-qt filename [-extract]\n");
    exit(1);
  }
  
  Poppler::Document *doc = Poppler::Document::load(argv[1]);
  if (!doc)
  {
    printf("doc not loaded\n");
    exit(1);
  }
  
  if (argc == 2)
  {  
    PDFDisplay test( doc );        // create picture display
    a.setMainWidget( &test);                // set main widget
    test.setCaption("Poppler-Qt Test");
    test.show();                            // show it

    return a.exec();                        // start event loop
  }
  else
  {
    Poppler::Page *page = doc->getPage(0);

    QLabel *l = new QLabel(page->getText(Poppler::Rectangle()), 0);
    l->show();
    a.setMainWidget(l);                // set main widget
    delete page;
    delete doc;
    return a.exec();
  }
}