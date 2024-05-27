#include "mainwindow.h"

#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Constructs a new main window.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
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
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_valueChanged Updates the opening angle of the cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setAngle(value);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_valueChanged Updates the height (length) of the cylinder.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setHeight(value);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_x_valueChanged Updates the a coefficient of the x(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_x_valueChanged(int value) {
  qDebug() << "a of x(t) updated to: " << value;

  Polynomial x = ui->mainView->path.getX();
  x.setA(value);

  ui->mainView->path.setX(x);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_x_valueChanged Updates the b coefficient of the x(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_x_valueChanged(int value) {
  qDebug() << "b of x(t) updated to: " << value;

  Polynomial x = ui->mainView->path.getX();
  x.setB(value);

  ui->mainView->path.setX(x);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_x_valueChanged Updates the c coefficient of the x(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_x_valueChanged(int value) {
  qDebug() << "c of x(t) updated to: " << value;

  Polynomial x = ui->mainView->path.getX();
  x.setC(value);

  ui->mainView->path.setX(x);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_y_valueChanged Updates the a coefficient of the y(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_y_valueChanged(int value) {
  qDebug() << "a of y(t) updated to: " << value;

  Polynomial y = ui->mainView->path.getY();
  y.setA(value);

  ui->mainView->path.setY(y);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_y_valueChanged Updates the b coefficient of the y(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_y_valueChanged(int value) {
  qDebug() << "b of y(t) updated to: " << value;

  Polynomial y = ui->mainView->path.getY();
  y.setB(value);

  ui->mainView->path.setY(y);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_y_valueChanged Updates the c coefficient of the y(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_y_valueChanged(int value) {
  qDebug() << "c of y(t) updated to: " << value;

  Polynomial y = ui->mainView->path.getY();
  y.setC(value);

  ui->mainView->path.setY(y);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_spinBox_a_z_valueChanged Updates the a coefficient of the z(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_z_valueChanged(int value) {
  qDebug() << "a of z(t) updated to: " << value;

  Polynomial z = ui->mainView->path.getZ();
  z.setA(value);

  ui->mainView->path.setZ(z);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_z_valueChanged Updates the b coefficient of the z(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_z_valueChanged(int value) {
  qDebug() << "b of z(t) updated to: " << value;

  Polynomial z = ui->mainView->path.getZ();
  z.setB(value);

  ui->mainView->path.setZ(z);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_z_valueChanged Updates the c coefficient of the z(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_z_valueChanged(int value) {
  qDebug() << "c of z(t) updated to: " << value;

  Polynomial z = ui->mainView->path.getZ();
  z.setC(value);

  ui->mainView->path.setZ(z);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_t_0_valueChanged Updates the lower bound of the time range of the path.
 * @param value The new lower bound.
 */
void MainWindow::on_spinBox_t_0_valueChanged(int value) {
  qDebug() << "t0 updated to: " << value;

  ui->mainView->path.setRange(value, ui->mainView->path.getT1());
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_t_1_valueChanged Updates the upper bound of the time range of the path.
 * @param value The new upper bound.
 */
void MainWindow::on_spinBox_t_1_valueChanged(int value) {
  qDebug() << "t1 updated to: " << value;

  ui->mainView->path.setRange(ui->mainView->path.getT0(), value);
  ui->mainView->move.setPath(ui->mainView->path);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_TimeSlider_sliderMoved Updates the time value.
 * @param value The new time value.
 */
void MainWindow::on_TimeSlider_sliderMoved(int value) {
  float divisor = ui->mainView->path.getSectors()/ ui->mainView->path.getRange();
  ui->mainView->setTime((value +1) / divisor);
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
