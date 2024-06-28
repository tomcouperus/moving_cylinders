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

void MainWindow::on_secondPassCheckBox_toggled(bool checked){
  ui->mainView->settings.secondEnv = checked;

  ui->tanContCheckBox->setEnabled(checked);
  ui->angleSpinBox_2->setEnabled(checked && ui->mainView->settings.tool2Idx==0);
  ui->heightSpinBox_2->setEnabled(checked);
  ui->radiusSpinBox_2->setEnabled(checked);
  ui->radius0SpinBox_2->setEnabled(checked && ui->mainView->settings.tool2Idx==1);
  ui->orientVector_1_2->setEnabled(checked && !ui->tanContCheckBox->isChecked());
  ui->orientVector_2_2->setEnabled(checked && !ui->tanContCheckBox->isChecked());
  ui->angleOrient_1_SpinBox->setEnabled(checked && ui->tanContCheckBox->isChecked());
  ui->angleOrient_2_SpinBox->setEnabled(checked && ui->tanContCheckBox->isChecked());
  ui->toolBox_2->setEnabled(checked);

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radiusSpinBox_valueChanged Updates the radius of the tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setRadius(value);
  ui->mainView->drum.setMidRadius(value);
  ui->radius0SpinBox->setMinimum(ui->mainView->drum.getMinR0());

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinder2.setRadius(value);
    ui->radiusSpinBox_2->setValue(value);
    ui->mainView->drum2.setMidRadius(value);
    ui->radius0SpinBox_2->setMinimum(ui->mainView->drum2.getMinR0());
  }

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radius0SpinBox_valueChanged Updates the inner radius of the drum.
 * @param value new inner radius.
 */
void MainWindow::on_radius0SpinBox_valueChanged(double value) {
  ui->mainView->drum.setRadius(value);

  if (ui->mainView->settings.secondEnv){
    ui->mainView->drum2.setRadius(value);
    ui->radius0SpinBox_2->setValue(value);
  }

  ui->mainView->updateToolTransf();
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

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinder2.setAngle(value);
    ui->angleSpinBox_2->setValue(value);
    ui->mainView->envelope2.setTool(&(ui->mainView->cylinder2));
  }

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_valueChanged Updates the height (length) of the tool.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_valueChanged(double value) {
  ui->mainView->cylinder.setHeight(value);
  ui->mainView->drum.setHeight(value);
  ui->radius0SpinBox->setMinimum(ui->mainView->drum.getMinR0());

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinder2.setHeight(value);
    ui->heightSpinBox_2->setValue(value);
    ui->mainView->drum2.setHeight(value);
    ui->radius0SpinBox_2->setMinimum(ui->mainView->drum2.getMinR0());

    ui->mainView->envelope.setTool(&(ui->mainView->cylinder));
    ui->mainView->envelope2.setTool(&(ui->mainView->cylinder2));
  }

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientVector_1_returnPressed Updates the orientation vector of the tool.
 */
void MainWindow::on_orientVector_1_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 1 vector");
  }

  if (ui->mainView->settings.secondEnv){
      ui->orientVector_1_2->setText(ui->orientVector_1->text());
      ui->orientVector_2_2->setText(ui->orientVector_2->text());

      vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1_2->text());
      vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2_2->text());

      success = ui->mainView->move2.setAxisDirections(vector1,vector2);
      if (success){
          ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
      } else {
          error.showMessage("The inputed vector is not a valid orientation 1 vector");
      }
  }

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientVector_2_returnPressed Updates the orientation vector of the tool.
 */
void MainWindow::on_orientVector_2_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 2 vector");
  }

  if (ui->mainView->settings.secondEnv){
      ui->orientVector_1_2->setText(ui->orientVector_1->text());
      ui->orientVector_2_2->setText(ui->orientVector_2->text());

      vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1_2->text());
      vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2_2->text());

      success = ui->mainView->move2.setAxisDirections(vector1,vector2);
      if (success){
          ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
      } else {
          error.showMessage("The inputed vector is not a valid orientation 2 vector");
      }
  }

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolBox_currentIndexChanged Updates the tool type. Activates/Deactivates their corresponding input fields.
 * @param index The index of the tool.
 */
void MainWindow::on_toolBox_currentIndexChanged(int index){
  qDebug() << "tool index" << index;
  ui->mainView->settings.toolIdx = index;
  switch (index) {
  case 0:
      ui->angleSpinBox->setEnabled(true);
      ui->radius0SpinBox->setEnabled(false);
      break;
  case 1:
      ui->angleSpinBox->setEnabled(false);
      ui->radius0SpinBox->setEnabled(true);
      break;
  default:
      ui->angleSpinBox->setEnabled(false);
      ui->radius0SpinBox->setEnabled(false);
      break;
  }
  ui->aSlider->setValue(0);
  ui->mainView->setA(0);

  if (ui->mainView->settings.secondEnv){
      ui->toolBox_2->setCurrentIndex(index);

      switch (index) {
      case 0:
          ui->angleSpinBox_2->setEnabled(true);
          ui->radius0SpinBox_2->setEnabled(false);
          break;
      case 1:
          ui->angleSpinBox_2->setEnabled(false);
          ui->radius0SpinBox_2->setEnabled(true);
          break;
      default:
          ui->angleSpinBox_2->setEnabled(false);
          ui->radius0SpinBox_2->setEnabled(false);
          break;
      }
  }

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radiusSpinBox_2_valueChanged Updates the radius of the second tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinder2.setRadius(value);
  ui->mainView->drum2.setMidRadius(value);
  ui->radius0SpinBox_2->setMinimum(ui->mainView->drum2.getMinR0());

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radius0SpinBox_2_valueChanged Updates the inner radius of the second drum.
 * @param value new inner radius.
 */
void MainWindow::on_radius0SpinBox_2_valueChanged(double value) {
  ui->mainView->drum2.setRadius(value);

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_2_valueChanged Updates the opening angle of the second cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinder2.setAngle(value);

  ui->mainView->envelope2.setTool(&(ui->mainView->cylinder2));

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_2_valueChanged Updates the height (length) of the second tool.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinder2.setHeight(value);
  ui->mainView->drum2.setHeight(value);
  ui->radius0SpinBox_2->setMinimum(ui->mainView->drum2.getMinR0());

  ui->mainView->envelope2.setTool(&(ui->mainView->cylinder2));

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientVector_1_2_returnPressed Updates the orientation vector of the second tool.
 */
void MainWindow::on_orientVector_1_2_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1_2->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move2.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 1 vector");
  }

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientVector_2_2_returnPressed Updates the orientation vector of the second tool.
 */
void MainWindow::on_orientVector_2_2_returnPressed(){
  qDebug() << "orientation vector changed";
  QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1_2->text());
  QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2_2->text());

  qDebug() << "to" << vector1 << "and" << vector2;

  bool success = ui->mainView->move2.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 2 vector");
  }

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolBox_2_currentIndexChanged Updates the second tool type. 
 * Activates/Deactivates their corresponding input fields.
 * @param index The index of the tool.
 */
void MainWindow::on_toolBox_2_currentIndexChanged(int index){
  qDebug() << "tool index" << index;
  ui->mainView->settings.tool2Idx = index;
  switch (index) {
  case 0:
      ui->angleSpinBox_2->setEnabled(true);
      ui->radius0SpinBox_2->setEnabled(false);
      break;
  case 1:
      ui->angleSpinBox_2->setEnabled(false);
      ui->radius0SpinBox_2->setEnabled(true);
      break;
  default:
      ui->angleSpinBox_2->setEnabled(false);
      ui->radius0SpinBox_2->setEnabled(false);
      break;
  }
  ui->aSlider->setValue(0);
  ui->mainView->setA(0);

  ui->mainView->updateBuffers();
  ui->mainView->updateAdjToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_tanContCheckBox_toggled Updates the tangetial continuity settings of the envelopes.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_tanContCheckBox_toggled(bool checked){
  ui->mainView->envelope2.setIsTanContinuous(checked);

  ui->angleOrient_1_SpinBox->setEnabled(checked);
  ui->angleOrient_2_SpinBox->setEnabled(checked);
  ui->orientVector_1_2->setEnabled(!checked);
  ui->orientVector_2_2->setEnabled(!checked);

  QVector3D vector1;
  QVector3D vector2;
  if(checked) {
      vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
      vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());
  } else {
      vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1_2->text());
      vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2_2->text());
  }
  bool success = ui->mainView->move2.setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
  }

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleOrient_1_SpinBox_valueChanged Updates the angle of the axis initial orientation of the
 * second tool with respect to the one of the first tool.
 * @param value new angle.
 */ 
void MainWindow::on_angleOrient_1_SpinBox_valueChanged(double value) {
  ui->mainView->envelope2.setAdjacentAxisAngles(value, ui->angleOrient_2_SpinBox->value());

  ui->mainView->updateAdjToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleOrient_2_SpinBox_valueChanged Updates the angle of the axis final orientation of the
 * second tool with respect to the one of the first tool.
 * @param value new angle.
 */
void MainWindow::on_angleOrient_2_SpinBox_valueChanged(double value) {
  ui->mainView->envelope2.setAdjacentAxisAngles(ui->angleOrient_1_SpinBox->value(), value);

  ui->mainView->updateAdjToolTransf();
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);
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

  ui->spinBox_t_1->setMinimum(value+1);

  SimplePath path = ui->mainView->move.getPath();
  float sliderTimePerSector = (ui->mainView->settings.time-path.getT0()) / (path.getRange());

  path.setRange(value, path.getT1());
  ui->mainView->move.setPath(path);
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);

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

  ui->spinBox_t_0->setMaximum(value-1);

  SimplePath path = ui->mainView->move.getPath();
  float sliderTimePerSector = (ui->mainView->settings.time-path.getT0()) / (path.getRange());

  path.setRange(path.getT0(), value);
  ui->mainView->move.setPath(path);
  ui->mainView->move2.setPath(path);

  ui->mainView->envelope.setToolMovement(&ui->mainView->move);
  ui->mainView->envelope2.setToolMovement(&ui->mainView->move2);

  ui->mainView->setTime(sliderTimePerSector*path.getRange());

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_envelopeCheckBox_toggled Updates the envelope(s) visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_envelopeCheckBox_toggled(bool checked){
  ui->mainView->settings.showEnvelope = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolCheckBox_toggled Updates the tool(s) visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_toolCheckBox_toggled(bool checked){
  ui->mainView->settings.showTool = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_grazCurveCheckBox_toggled Updates the grazing curves visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_grazCurveCheckBox_toggled(bool checked){
  ui->mainView->settings.showGrazingCurve = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_pathCheckBox_toggled Updates the path visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_pathCheckBox_toggled(bool checked){
  ui->mainView->settings.showPath = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolAxisCheckBox_toggled Updates the tool axis visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_toolAxisCheckBox_toggled(bool checked){
  ui->mainView->settings.showToolAxis = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_normalsCheckBox_toggled Updates the normals visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_normalsCheckBox_toggled(bool checked){
  ui->mainView->settings.showNormals = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_sphereCheckBox_toggled Updates the spheres visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_sphereCheckBox_toggled(bool checked){
  ui->mainView->settings.showSpheres = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_TimeSlider_sliderMoved Updates the time value.
 * @param value The new time value.
 */
void MainWindow::on_TimeSlider_sliderMoved(int value) {
  SimplePath path = ui->mainView->move.getPath();
  float divisor = path.getSectors() / (path.getRange());
  ui->mainView->setTime(value / divisor);
  ui->mainView->updateBuffers();
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_aSlider_sliderMoved Updates the time value for the tool axis.
 * @param value The new a value.
 */
void MainWindow::on_aSlider_sliderMoved(int value) {
  ui->mainView->setA(value);

  ui->mainView->updateBuffers();
  ui->mainView->update();
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
