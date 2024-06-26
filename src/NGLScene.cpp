#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>
#include <ngl/MultiBufferVAO.h>
#include <ngl/SimpleVAO.h>
#include <string>
#include <ngl/ShaderLib.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ngl/Util.h>
#include <chrono>


int g_axis;

NGLScene::NGLScene()
{
  setTitle("Using ngl::BezierCurve");
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(int _w, int _h)
{
  m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.05f, 350.0f);
  m_win.width = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
  // m_text->setScreenSize(_w,_h);
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();
  mesh = Mesh(4);
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0.0f, 1.0f, 15.0f);
  ngl::Vec3 to(0.0f, 0.0f, 0.0f);
  ngl::Vec3 up(0.0f, 1.0f, 0.0f);
  m_view = ngl::lookAt(from, to, up);
  m_project = ngl::perspective(45.0f, 720.0f / 576.0f, 0.5f, 150.0f);
  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("Colour", 1.0f, 1.0f, 1.0f, 1.0f);
  mesh.setVel(-0.2, 1,1);
  startTimer(10);
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  auto updateBegin = std::chrono::steady_clock::now();
  mesh.updateChain();
  auto updateEnd = std::chrono::steady_clock::now();
  m_updateTime =
  std::chrono::duration_cast<std::chrono::microseconds>(updateEnd-updateBegin).count();
  update();
}

void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::Mat4 MVP;
  MVP = m_project * m_view * m_mouseGlobalTX;
  ngl::ShaderLib::setUniform("MVP", MVP);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_win.width, m_win.height);
  // Rotation based on the mouse position for our global transform
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  // multiply the rotations
  m_mouseGlobalTX = rotY * rotX;
  // add the translations
  drawline();

  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
  loadMatricesToShader();


}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent(QMouseEvent *_event)
{
// note the method buttons() is the button state when event was called
// that is different from button() which is used to check which button was
// pressed when the mousePress/Release event is generated
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif

  if (m_win.rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx = position.x() - m_win.origX;
    int diffy = position.y() - m_win.origY;
    m_win.spinXFace += static_cast<int>(0.5f * diffy);
    m_win.spinYFace += static_cast<int>(0.5f * diffx);
    m_win.origX = position.x();
    m_win.origY = position.y();
    update();
  }
  // right mouse translate code
  else if (m_win.translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = static_cast<int>(position.x() - m_win.origXPos);
    int diffY = static_cast<int>(position.y() - m_win.origYPos);
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent(QMouseEvent *_event)
{
// that method is called when the mouse button is pressed in this case we
// store the value where the maouse was clicked (x,y) and set the Rotate flag to true
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif

  if (_event->button() == Qt::LeftButton)
  {
    m_win.origX = position.x();
    m_win.origY = position.y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if (_event->button() == Qt::RightButton)
  {
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent(QMouseEvent *_event)
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if (_event->angleDelta().x() > 0)
  {
    m_modelPos.m_z += ZOOM;
  }
  else if (_event->angleDelta().x() < 0)
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape:
    QGuiApplication::exit(EXIT_SUCCESS);
    break;
  case Qt::Key_W:{
    mesh.setVel(0.2, 1, 1);
    break;
  }
  case Qt::Key_A:{
    mesh.setVel(-0.2, 1, 0);
    break;
  }

  case Qt::Key_S:{
    mesh.setVel(-0.2, 1,1);
    break;
  }

  case Qt::Key_D:{
    mesh.setVel(0.2, 1, 0);
    break;
  }
  case Qt::Key_I:{
    mesh.setBeg(0.2, 0, 1);
    break;
  }
    case Qt::Key_J:{
    mesh.setBeg(-0.2, 0, 0);
    break;
  }
    case Qt::Key_K:{
    mesh.setBeg(-0.2, 0, 1);
    break;
  }
    case Qt::Key_L:{
    mesh.setBeg(0.2, 0, 0);
    break;
  }
  case Qt::Key_1:{
    sup = !sup;
    break;
  }
  case Qt::Key_2:{
    mesh.setSupB();
    break;
  }
  default:
    break;
  
  }
  update();
}
void NGLScene::drawline(){
    std::unique_ptr<ngl::MultiBufferVAO> m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_LINES));
    m_vao->bind();
    std::vector<ngl::Vec3> verts = mesh.getChain();
  // std::array<ngl::Vec3, > verts =
  //     {{ngl::Vec3(0.0f, 1.0f, 1.0f),
  //       ngl::Vec3(0.0f, 0.0f, -1.0f)}};
  m_vao->setData(ngl::SimpleVAO::VertexData(verts.size() * sizeof(ngl::Vec3), verts[0].m_x));
  m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);
  // divide by 2 as we have both verts and normals
  m_vao->setNumIndices(verts.size());
  m_vao->setMode(GL_LINES);
  m_vao->draw();
  if(sup == true){
  verts = mesh.getSup();
  m_vao->setData(ngl::SimpleVAO::VertexData(verts.size() * sizeof(ngl::Vec3), verts[0].m_x));
  m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);
  // divide by 2 as we have both verts and normals

  m_vao->setNumIndices(verts.size());
  m_vao->setMode(GL_LINES);
  m_vao->draw();
  }
  m_vao->unbind();
  
}