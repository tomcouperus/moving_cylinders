#include "mainwindow.h"

#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Constructs a new main window.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->orientVector_1->setValidator(new QRegularExpressionValidator(
      QRegularExpression("\\(\\-?(\\d*\\.?\\d+),\\-?(\\d*\\.?\\d+),\\-?(\\d*\\.?\\d+)\\)")));
  ui->orientVector_2->setValidator(new QRegularExpressionValidator(
      QRegularExpression("\\(\\-?(\\d*\\.?\\d+),\\-?(\\d*\\.?\\d+),\\-?(\\d*\\.?\\d+)\\)")));

  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

  ui->mainView->move.setAxisDirections(vector1,vector2);
}

/**
 * @brief MainWindow::~MainWindow Destructor.
 */
MainWindow::~MainWindow() { delete ui; }

/**
 * @brief MainWindow::on_radiusSpinBox_valueChanged Updates the radius of the cylinder.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setRadius(value);
  ui->mainView->drum.setMidRadius(value);
  ui->radius0SpinBox->setMinimum(ui->mainView->drum.minR0with(value));

  ui->mainView->envelope.setTool(&(ui->mainView->cylinder));
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

void MainWindow::on_radius0SpinBox_valueChanged(double value) {
  ui->mainView->drum.setRadius(value);

  ui->mainView->envelope.setTool(&(ui->mainView->cylinder));
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_valueChanged Updates the opening angle of the cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setAngle(value);

  ui->mainView->envelope.setTool(&(ui->mainView->cylinder));
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_valueChanged Updates the height (length) of the cylinder.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setHeight(value);
  ui->mainView->drum.setHeight(value);

  ui->mainView->envelope.setTool(&(ui->mainView->cylinder));
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

void MainWindow::on_orientVector_1_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope.setToolMovement(ui->mainView->move);
      ui->mainView->updateToolTransf();
  } else {
      error.showMessage("The inputed vector is not a valid orientation 1 vector");
  }

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

void MainWindow::on_orientVector_2_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope.setToolMovement(ui->mainView->move);
      ui->mainView->updateToolTransf();
  } else {
      error.showMessage("The inputed vector is not a valid orientation 2 vector");
  }

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

void MainWindow::on_toolBox_currentIndexChanged(int index){
  qDebug() << "tool index" << index;
  switch (index) {
  case 0:
      ui->mainView->settings.isCylinder = true;
      ui->angleSpinBox->setEnabled(true);
      ui->radius0SpinBox->setEnabled(false);
      break;
  default:
      ui->mainView->settings.isCylinder = false;
      ui->angleSpinBox->setEnabled(false);
      ui->radius0SpinBox->setEnabled(true);
      break;
  }
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_x_valueChanged Updates the a coefficient of the x(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_x_valueChanged(int value) {
  qDebug() << "a of x(t) updated to: " << value;


  SimplePath path = ui->mainView->move.getPath();
  Polynomial x = path.getX();
  x.setA(value);

  path.setX(x);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_x_valueChanged Updates the b coefficient of the x(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_x_valueChanged(int value) {
  qDebug() << "b of x(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial x = path.getX();
  x.setB(value);

  path.setX(x);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_x_valueChanged Updates the c coefficient of the x(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_x_valueChanged(int value) {
  qDebug() << "c of x(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial x = path.getX();
  x.setC(value);

  path.setX(x);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_y_valueChanged Updates the a coefficient of the y(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_y_valueChanged(int value) {
  qDebug() << "a of y(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial y = path.getY();
  y.setA(value);

  path.setY(y);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_y_valueChanged Updates the b coefficient of the y(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_y_valueChanged(int value) {
  qDebug() << "b of y(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial y = path.getY();
  y.setB(value);

  path.setY(y);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_y_valueChanged Updates the c coefficient of the y(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_y_valueChanged(int value) {
  qDebug() << "c of y(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial y = path.getY();
  y.setC(value);

  path.setY(y);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_spinBox_a_z_valueChanged Updates the a coefficient of the z(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_z_valueChanged(int value) {
  qDebug() << "a of z(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial z = path.getZ();
  z.setA(value);

  path.setZ(z);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_z_valueChanged Updates the b coefficient of the z(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_z_valueChanged(int value) {
  qDebug() << "b of z(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial z = path.getZ();
  z.setB(value);

  path.setZ(z);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_z_valueChanged Updates the c coefficient of the z(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_z_valueChanged(int value) {
  qDebug() << "c of z(t) updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  Polynomial z = path.getZ();
  z.setC(value);

  path.setZ(z);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);
  ui->mainView->updateToolTransf();

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_t_0_valueChanged Updates the lower bound of the time range of the path.
 * @param value The new lower bound.
 */
void MainWindow::on_spinBox_t_0_valueChanged(int value) {
  qDebug() << "t0 updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  float sliderTimePerSector = (ui->mainView->time-path.getT0()) / path.getRange();

  path.setRange(value, path.getT1());
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);

  ui->mainView->setTime(sliderTimePerSector*path.getRange());

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_t_1_valueChanged Updates the upper bound of the time range of the path.
 * @param value The new upper bound.
 */
void MainWindow::on_spinBox_t_1_valueChanged(int value) {
  qDebug() << "t1 updated to: " << value;

  SimplePath path = ui->mainView->move.getPath();
  float sliderTimePerSector = (ui->mainView->time-path.getT0()) / path.getRange();

  path.setRange(path.getT0(), value);
  ui->mainView->move.setPath(path);

  ui->mainView->envelope.setToolMovement(ui->mainView->move);

  ui->mainView->setTime(sliderTimePerSector*path.getRange());

  ui->mainView->updateBuffers();
  ui->mainView->update();
}


void MainWindow::on_envelopeCheckBox_toggled(bool checked){
  ui->mainView->settings.showEnvelope = checked;
  ui->mainView->update();
}

void MainWindow::on_grazCurveCheckBox_toggled(bool checked){
  ui->mainView->settings.showGrazingCurve = checked;
  ui->mainView->update();
}

void MainWindow::on_pathCheckBox_toggled(bool checked){
  ui->mainView->settings.showPath = checked;
  ui->mainView->update();
}

void MainWindow::on_toolAxisCheckBox_toggled(bool checked){
  ui->mainView->settings.showToolAxis = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_TimeSlider_sliderMoved Updates the time value.
 * @param value The new time value.
 */
void MainWindow::on_TimeSlider_sliderMoved(int value) {
  SimplePath path = ui->mainView->move.getPath();
  float divisor = path.getSectors()/ path.getRange();
  ui->mainView->setTime(value / divisor);
}


/**
 * @brief MainWindow::on_ResetRotationButton_clicked Resets the rotation.
 * @param checked Unused.
 */
void MainWindow::on_ResetRotationButton_clicked(bool checked) {
  Q_UNUSED(checked)
  ui->RotationDialX->setValue(0);
  ui->RotationDialY->setValue(0);
  ui->RotationDialZ->setValue(0);
  ui->mainView->setRotation(0, 0, 0);
}

/**
 * @brief MainWindow::on_RotationDialX_sliderMoved Updates the number of degrees
 * of rotation in the x direction.
 * @param value Unused.
 */
void MainWindow::on_RotationDialX_sliderMoved(int value) {
  ui->mainView->setRotation(value, ui->RotationDialY->value(),
                            ui->RotationDialZ->value());
}

/**
 * @brief MainWindow::on_RotationDialY_sliderMoved Updates the number of degrees
 * of rotation in the y direction.
 * @param value Unused.
 */
void MainWindow::on_RotationDialY_sliderMoved(int value) {
  ui->mainView->setRotation(ui->RotationDialX->value(), value,
                            ui->RotationDialZ->value());
}

/**
 * @brief MainWindow::on_RotationDialZ_sliderMoved Updates the number of degrees
 * of rotation in the z direction.
 * @param value Unused.
 */
void MainWindow::on_RotationDialZ_sliderMoved(int value) {
  ui->mainView->setRotation(ui->RotationDialX->value(),
                            ui->RotationDialY->value(), value);
}

/**
 * @brief MainWindow::on_ResetScaleButton_clicked Resets the scale factor.
 * @param checked Unused.
 */
void MainWindow::on_ResetScaleButton_clicked(bool checked) {
  Q_UNUSED(checked)
  ui->ScaleSlider->setValue(100);
  ui->mainView->setScale(1);
}

/**
 * @brief MainWindow::on_ScaleSlider_sliderMoved Updates the scale value.
 * @param value The new scale value.
 */
void MainWindow::on_ScaleSlider_sliderMoved(int value) {
  ui->mainView->setScale(value / 100.0f);
}

/**
 * @brief MainWindow::renderToFile Used to render the frame buffer to the file.
 * DO NOT REMOVE OR MODIFY!
 */
void MainWindow::renderToFile() {
  QImage image = ui->mainView->grabFramebuffer();
  image.save("framebuffer_contents.png", "PNG",
             100);  // Save using the best quality
}
