//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    glwidget.cpp
  \brief   C++ Implementation: GLWidget
  \author  Stefan Zickler, (C) 2008
*/
//========================================================================

#include "glwidget.h"

void GLWidget::mouseAction ( QMouseEvent * event, pixelloc loc ) {
  (void)loc;
  if ( ( event->buttons() & Qt::RightButton ) !=0 ) {
    //Right mouse button...drag image
    QPoint offset=mouseStart-event->pos();
    zoom.setPan ( mouseStartPanX- ( ( double ) offset.x() / ( zoom.getZoom() * zoom.getFlipXval() * vpW ) ),
                  mouseStartPanY- ( ( double ) offset.y() / ( zoom.getZoom() * zoom.getFlipYval() * vpH ) ) );
  } /* else if ( ( event->buttons() & Qt::LeftButton ) !=0 ) {
    //Left mouse button...color-pick from image.
    if ( rb!=0 ) {
      rb->lockRead();
      int idx=rb->curRead();
      FrameData * frame = rb->getPointer ( idx );
      VisualizationFrame * vis_frame=(VisualizationFrame *)(frame->map.get("vis_frame"));
      if (vis_frame!=0 && vis_frame->valid==true) {

        rgbImage & img = vis_frame->data;

        if ( loc.x < img.getWidth() && loc.y < img.getHeight() && loc.x >=0 && loc.y >=0 ) {
          if ( img.getWidth() > 1 && img.getHeight() > 1 ) {
            colorPicker->setColor ( img.getPixel ( loc.x,loc.y ) );
            //img.setPixel(loc.x,loc.y,rgb(255,0,0));
          }
        }
      }
      rb->unlockRead();
    }
  }*/
  redraw();
}

void GLWidget::mousePressEvent ( QMouseEvent * event ) {
  event->setAccepted ( false );
  pixelloc loc=zoom.invZoom ( event->pos().x(),event->pos().y(),true );
  if ( stack!=0 ) stack->mousePressEvent ( event,loc );
  if ( event->isAccepted() ==true ) return;
  mouseStart=event->pos();
  mouseStartPanX=zoom.getPanX();
  mouseStartPanY=zoom.getPanY();
  mouseAction ( event,loc );
}

void GLWidget::mouseReleaseEvent ( QMouseEvent * event ) {
  event->setAccepted ( false );
  pixelloc loc=zoom.invZoom ( event->pos().x(),event->pos().y(),true );
  if ( stack!=0 ) stack->mouseReleaseEvent ( event,loc );
  if ( event->isAccepted() ==true ) return;
  mouseAction ( event,loc );
}

void GLWidget::mouseMoveEvent ( QMouseEvent * event ) {
  event->setAccepted ( false );
  pixelloc loc=zoom.invZoom ( event->pos().x(),event->pos().y(),true );
  if ( stack!=0 ) stack->mouseMoveEvent ( event,loc );
  if ( event->isAccepted() ==true ) return;
  mouseAction ( event,loc );
}


GLWidget::GLWidget ( QWidget *parent ) : QGLWidget ( parent ) {
  rb_bb=0;
  rb=0;
  stack=0;

  //not needed because we are remote triggering this:
  //startTimer(1);

  /*setMinimumSize ( 60,40 );
  colorPicker=new ColorPicker();
  actionColorPicker=new QWidgetAction ( this );
  actionColorPicker->setDefaultWidget ( ( QWidget * ) colorPicker );
  actionColorPicker->setVisible ( true );
  actionColorPicker->setToolTip ( "RGB Color Picker\n(Left click in image to use)" );
  addAction ( actionColorPicker );*/

  QAction * actionSave = new QAction ( this );
  actionSave->setObjectName ( "actionSave" );
  actionSave->setIcon ( QIcon ( ":/icons/document-save.png" ) );
  actionSave->setShortcut ( QKeySequence ( "Ctrl+s" ) );
  actionSave->setToolTip ( "Save Image... (Ctrl-s)" );
  actionSave->setShortcutContext ( Qt::WidgetShortcut );
  addAction ( actionSave );


  actionFlipV = new QAction ( this );
  actionFlipV->setObjectName ( "actionFlipV" );
  actionFlipV->setToolTip ( "Flip image vertically" );
  actionFlipV->setCheckable ( true );
  actionFlipV->setIcon ( QIcon ( ":/icons/delete_table_row.png" ) );
  addAction ( actionFlipV );

  actionFlipH = new QAction ( this );
  actionFlipH->setObjectName ( "actionFlipH" );
  actionFlipH->setToolTip ( "Flip image horizontally" );
  actionFlipH->setCheckable ( true );
  actionFlipH->setIcon ( QIcon ( ":/icons/delete_table_col.png" ) );
  addAction ( actionFlipH );

  QAction * actionHelp = new QAction ( this );
  actionHelp->setObjectName ( "actionHelp" );
  actionHelp->setIcon ( QIcon ( ":/icons/help-contents.png" ) );
  actionHelp->setShortcut ( QKeySequence ( "Ctrl+h" ) );
  actionHelp->setToolTip ( "Show keyboard shortcuts (Ctrl-h)" );
  actionHelp->setShortcutContext ( Qt::WidgetShortcut );
  addAction ( actionHelp );

  QAction * actionZoomFit = new QAction ( this );
  actionZoomFit->setObjectName ( "actionZoomFit" );
  actionZoomFit->setIcon ( QIcon ( ":/icons/zoom-best-fit.png" ) );
  actionZoomFit->setShortcut ( QKeySequence ( "Space" ) );
  actionZoomFit->setToolTip ( "Zoom to Fit (Space)" );
  actionZoomFit->setShortcutContext ( Qt::WidgetShortcut );
  addAction ( actionZoomFit );

  QAction * actionZoomNormal = new QAction ( this );
  actionZoomNormal->setObjectName ( "actionZoomNormal" );
  actionZoomNormal->setIcon ( QIcon ( ":/icons/zoom-original.png" ) );
  actionZoomNormal->setShortcut ( QKeySequence ( "Home" ) );
  actionZoomNormal->setShortcutContext ( Qt::WidgetShortcut );
  actionZoomNormal->setToolTip ( "Zoom to 100% (Home)" );
  addAction ( actionZoomNormal );

  actionOn = new QAction ( this );
  actionOn->setObjectName ( "actionOn" );
  actionOn->setCheckable ( true );
  actionOn->setChecked ( true );
  actionOn->setShortcut ( QKeySequence ( "Ctrl+p" ) );
  actionOn->setShortcutContext ( Qt::WidgetShortcut );
  actionOn->setToolTip ( "Turn display on/off (Ctrl-p)" );
  actionOn->setIcon ( QIcon ( ":/icons/application-exit.png" ) );
  addAction ( actionOn );

  connect ( actionSave, SIGNAL ( triggered() ), this, SLOT ( saveImage() ) );
  connect ( actionFlipV, SIGNAL ( triggered ( bool ) ), this, SLOT ( flipImage() ) );
  connect ( actionFlipH, SIGNAL ( triggered ( bool ) ), this, SLOT ( flipImage() ) );
  connect ( actionHelp, SIGNAL ( triggered() ), this, SLOT ( callHelp() ) );
  connect ( actionZoomFit, SIGNAL ( triggered() ), this, SLOT ( callZoomFit() ) );
  connect ( actionZoomNormal, SIGNAL ( triggered() ), this, SLOT ( callZoomNormal() ) );


  flipImage();
}

GLWidget::~GLWidget() {
  //delete colorPicker;
}


void GLWidget::initializeGL() {
  qglClearColor ( QColor ( 64,64,128 ) );
  //DISABLE EVERYTHING
  //trying to make video-rendering as fast as possible
  glShadeModel ( GL_FLAT );
  glEnable ( GL_ALPHA_TEST );
  glEnable ( GL_BLEND );
  glEnable ( GL_STENCIL_TEST );
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glEnable ( GL_TEXTURE_2D );
  /*    glDisable(GL_DEPTH_TEST);
      glDisable(GL_FOG);
      glDisable(GL_LIGHTING);
      glDisable(GL_LOGIC_OP);
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_TEXTURE_1D);
      glDisable(GL_TEXTURE_2D);*/

  //dithering is a bit special
  //leave it on for now:
  //glDisable(GL_DITHER);

  //set Transfer mode to be as fast and direct as possible
  /* glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
   glPixelTransferi(GL_RED_SCALE, 1);
   glPixelTransferi(GL_RED_BIAS, 0);
   glPixelTransferi(GL_GREEN_SCALE, 1);
   glPixelTransferi(GL_GREEN_BIAS, 0);
   glPixelTransferi(GL_BLUE_SCALE, 1);
   glPixelTransferi(GL_BLUE_BIAS, 0);
   glPixelTransferi(GL_ALPHA_SCALE, 1);
   glPixelTransferi(GL_ALPHA_BIAS, 0);*/

}

void GLWidget::paintGL() {
  if ( actionOn->isChecked() ==false ) return;
  glPushAttrib ( GL_ALL_ATTRIB_BITS );
  glMatrixMode ( GL_PROJECTION );
  glLoadIdentity();
  glPushMatrix();
  glMatrixMode ( GL_MODELVIEW );
  glLoadIdentity();
  glPushMatrix();

  glClear ( GL_COLOR_BUFFER_BIT );
  if ( rb!=0 ) {
    rb->lockRead();
    int idx=rb->curRead();
    FrameData * frame = rb->getPointer ( idx );
    VisualizationFrame * vis_frame=(VisualizationFrame *)(frame->map.get("vis_frame"));
    if (vis_frame!=0 && vis_frame->valid==true && vis_frame->data.getData() != 0 && vis_frame->data.getWidth() >= 1 && vis_frame->data.getHeight() >=1 ) {
      rgbImage & img = vis_frame->data;
      if ( img.getWidth() > 1 && img.getHeight() > 1 ) {
        glPushMatrix();
        zoom.setup ( img.getWidth(), img.getHeight(), vpW,vpH,true );
        pixelloc orig=zoom.zoom ( 0, 0 );

        glPushMatrix();
        glRasterPos2i ( 0,0 );
        glBitmap ( 0,0,0,0,orig.x,-orig.y,0 );
        glPixelZoom ( zoom.getZoom() * zoom.getFlipXval(),zoom.getZoom() * zoom.getFlipYval() * -1.0 );
        glDrawPixels ( img.getWidth(), img.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, img.getData() );
        glPopMatrix();

        glPopMatrix();
      }

    }
    rb->unlockRead();
  }
  glPopAttrib();
  glMatrixMode ( GL_MODELVIEW );
  glPopMatrix();
  glMatrixMode ( GL_PROJECTION );
  glPopMatrix();

  c_draw.count();
  //updateVideoStats(stats);
}


void GLWidget::resizeGL ( int width, int height ) {

  glViewport ( 0, 0, width, height );
  glOrtho ( 0, width, 0, height, -1, 1 );

  //save new viewport dimensions:
  vpW=width;
  vpH=height;
  if ( vpW < 1 ) vpW=1;
  if ( vpH < 1 ) vpH=1;

}


void GLWidget::wheelEvent ( QWheelEvent * event ) {
  event->setAccepted ( false );
  pixelloc loc=zoom.invZoom ( event->pos().x(),event->pos().y(),true );
  if ( stack!=0 ) stack->wheelEvent ( event,loc );
  if ( event->isAccepted() ==true ) return;
  int delta=event->delta();
  if ( delta > 0 ) {
    this->zoom.zoomIn();
  } else {
    this->zoom.zoomOut();
  }
  this->redraw(); //upgl
  event->accept();
}

void GLWidget::callZoomNormal() {
  this->zoom.setPan ( 0.0,0.0 );
  this->zoom.setZoom ( 1.0 );
  this->redraw(); //upgl
}

void GLWidget::callZoomFit() {
  this->zoom.setPan ( 0.0,0.0 );
  this->zoom.zoomToFit();
  this->redraw(); //upgl
}

void GLWidget::callHelp() {
  QMessageBox::information ( this,"Keyboard and Mouse shortcuts",
                             "Left-Click: Pick RGB Color\nArrow-Keys (or Right-Drag Mouse): Panning\nPage Up/Down (or Mousewheel): Zooming\nHome: Reset Zoom to 100%\nSpace: Reset Zoom to Best Fit\nCtrl-b: Toggle Bounding Boxes\nCtrl-f: Toggle Fullscreen\nCtrl-w: Toggle Detach Window" );
}

void GLWidget::keyPressEvent ( QKeyEvent * event ) {
  event->setAccepted ( false );
  if ( stack!=0 ) stack->keyPressEvent ( event );
  if ( event->isAccepted() ==false ) {
    if ( event->key() ==Qt::Key_Left ) {
      this->zoom.panRight();
      this->redraw(); //upgl
      event->accept();
    } else if ( event->key() ==Qt::Key_Right ) {
      this->zoom.panLeft();
      this->redraw(); //upgl
      event->accept();
    } else if ( event->key() ==Qt::Key_Up ) {
      this->zoom.panDown();
      this->redraw(); //upgl
      event->accept();
    } else if ( event->key() ==Qt::Key_Down ) {
      this->zoom.panUp();
      this->redraw(); //upgl
      event->accept();
    } else if ( event->key() ==Qt::Key_PageUp ) {
      this->zoom.zoomIn();
      this->redraw(); //upgl
      event->accept();
    } else if ( event->key() ==Qt::Key_PageDown ) {
      this->zoom.zoomOut();
      this->redraw(); //upgl
      event->accept();
    } else {
      event->ignore();
    }
  }
}

void GLWidget::paintEvent ( QPaintEvent * e ) {
  ( void ) e;
  redraw();
}

void GLWidget::flipImage() {
  this->zoom.setFlipX ( actionFlipH->isChecked() );
  this->zoom.setFlipY ( actionFlipV->isChecked() );
  this->redraw(); //upgl
}

void GLWidget::saveImage() {
  rgbImage temp;
  if ( rb!=0 ) {
    rb->lockRead();
    int idx=rb->curRead();
    FrameData * frame = rb->getPointer ( idx );

    VisualizationFrame * vis_frame=(VisualizationFrame *)(frame->map.get("vis_frame"));
    if (vis_frame !=0 && vis_frame->valid) {
      temp.copy ( vis_frame->data );
      rb->unlockRead();
    } else {
      rb->unlockRead();
      return;
    }
  }
  if ( temp.getWidth() > 1 && temp.getHeight() > 1 ) {
    QFileDialog dialog ( this,
                         "Export image to file...",
                         "",
                         "PPM (*.ppm)" );
    dialog.setConfirmOverwrite ( true );
    dialog.setDefaultSuffix ( "ppm" );
    dialog.setAcceptMode ( QFileDialog::AcceptSave );
    if ( dialog.exec() ) {
      if ( dialog.selectedFiles().size() > 0 ) {
        QString filename=dialog.selectedFiles().at ( 0 );
        if ( temp.save ( filename.toStdString() ) ) {
          QMessageBox::information ( this, "Success","PPM was saved successfully." );
        } else {
          QMessageBox::warning ( this, "Error","Unknown error while saving ppm." );
        }
      }
    }
  }
}
