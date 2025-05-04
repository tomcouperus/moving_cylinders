#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <QStandardItemModel>

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

  // Populate the envelope select dropdown box here
  for (int i = 0; i < ui->mainView->envelopes.size(); i++) {
      QString text = "Envelope "+QString::number(i);
      ui->envelopeSelectBox->addItem(text);
      ui->constraintA0SelectBox->addItem(text);
      ui->constraintA1SelectBox->addItem(text);
  }

  on_envelopeSelectBox_currentIndexChanged(0);
}

/**
 * @brief MainWindow::~MainWindow Destructor.
 */
MainWindow::~MainWindow() { delete ui; }

/**
 * @brief SetComboBoxItemEnabled Changes the visibility of an item in a combobox. Thanks to https://stackoverflow.com/questions/38915001/disable-specific-items-in-qcombobox/62261745
 * @param comboBox
 * @param index
 * @param enabled
 */
void MainWindow::SetComboBoxItemEnabled(QComboBox *comboBox, int index, bool enabled) {
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

void MainWindow::on_envelopeSelectBox_currentIndexChanged(int index) {
    int idx = index-1;
    int prevIdx = ui->mainView->settings.selectedIdx;
    qDebug() << "Selected envelope" << idx;
    ui->mainView->settings.selectedIdx = idx;
    ui->mainView->settings.prevIdx = prevIdx;

    // Hide/unhide options from constraint selectors
    QVector<Envelope *> envelopes = ui->mainView->envelopes;
    if (prevIdx != -1) {
        SetComboBoxItemEnabled(ui->constraintA0SelectBox, prevIdx+1, true);
        SetComboBoxItemEnabled(ui->constraintA1SelectBox, prevIdx+1, true);
        if (envelopes[prevIdx]->isPositContinuous()) {
            SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelopes[prevIdx]->getAdjEnvelope()->getIndex()+1, true);
        }
    }

    if (idx == -1) {
        // Reset the ui elements
        // ** Active check box
        ui->envelopeActiveCheckBox->setChecked(false);
        // ** Constraint select boxes. Also need to unhide all the currently hidden options,
        // ** which always has to happen on a switch. So do that outside this if.
        ui->constraintA0SelectBox->setCurrentIndex(0);
        ui->constraintA1SelectBox->setCurrentIndex(0);
        ui->SettingsTabMenu->setTabEnabled(3, false);
    } else {
        Envelope *env = ui->mainView->envelopes[idx];
        // Load the selected envelope's data
        // ** Active check box
        ui->envelopeActiveCheckBox->setChecked(env->isActive());
        // ** Constraint select boxes. Also hide the current envelope
        SetComboBoxItemEnabled(ui->constraintA0SelectBox, idx+1, false);
        SetComboBoxItemEnabled(ui->constraintA1SelectBox, idx+1, false);
        int a0Idx = env->isPositContinuous() ? env->getAdjEnvelope()->getIndex()+1 : 0;
        ui->constraintA0SelectBox->setCurrentIndex(a0Idx);
        on_constraintA0SelectBox_currentIndexChanged(a0Idx);
    }

    // Enable/disable some ui elements
    bool envelopeSelected = idx != -1;
    ui->envelopeActiveCheckBox->setEnabled(envelopeSelected);
    ui->constraintsGroupBox->setEnabled(envelopeSelected);
    ui->SettingsTabMenu->setTabEnabled(2, envelopeSelected);

}

void MainWindow::on_envelopeActiveCheckBox_toggled(bool checked) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    if (ui->mainView->envelopes[idx]->isActive() == checked) return;

    qDebug() << "Changed active state of envelope" << idx;
    ui->mainView->envelopes[idx]->setActive(checked);

    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}


void MainWindow::on_constraintA0SelectBox_currentIndexChanged(int index) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    Envelope *envelope = ui->mainView->envelopes[idx];
    // Unhide previous adjacent envelope in a1 constraint
    if (envelope->isPositContinuous()) {
        SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelope->getAdjEnvelope()->getIndex()+1, true);
    }

    // Hide new adjacent envelope in a1 constraint, or if its no longer adjacent to any, disable the tangent and a1 constraint
    SetComboBoxItemEnabled(ui->constraintA1SelectBox, index, index == 0);
    ui->tanContCheckBox->setEnabled(index != 0);
    ui->constraintA1SelectBox->setEnabled(index != 0);
    // If adjacent, disable the path menu
    ui->SettingsTabMenu->setTabEnabled(3, index == 0);

    // Set adjacent envelope
    Envelope *adjEnv = (index == 0) ? nullptr : ui->mainView->envelopes[index-1];
    if (envelope->getAdjEnvelope() != adjEnv) {
        envelope->setAdjacentEnvelope(adjEnv);
        envelope->update();
        ui->mainView->updateBuffers();
        ui->mainView->updateToolTransf(); // TODO find way to target specific tools
        ui->mainView->updateUniformsRequired = true;
        ui->mainView->update();
    }

}

void MainWindow::on_constraintA1SelectBox_currentIndexChanged(int index) {

}


/**
 * @brief MainWindow::on_orientVector_1_returnPressed Updates the orientation vector of the tool.
 */
void MainWindow::on_orientVector_1_returnPressed(){
    int idx = ui->mainView->settings.selectedIdx;
    qDebug() << "orientation vector changed";
    QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
    QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

    qDebug() << "to" << vector1 << "and" << vector2;

    bool success = ui->mainView->movements[idx]->setAxisDirections(vector1,vector2);
    if (!success){
        error.showMessage("The inputed vector is not a valid orientation 1 vector");
    }

    ui->mainView->envelopes[idx]->update();
    ui->mainView->updateToolTransf();
    ui->mainView->updateBuffers();
    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientVector_2_returnPressed Updates the orientation vector of the tool.
 */
void MainWindow::on_orientVector_2_returnPressed(){
    int idx = ui->mainView->settings.selectedIdx;
    qDebug() << "orientation vector changed";
    QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
    QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

    qDebug() << "to" << vector1 << "and" << vector2;

    bool success = ui->mainView->movements[idx]->setAxisDirections(vector1,vector2);
    if (!success){
        error.showMessage("The inputed vector is not a valid orientation 2 vector");
    }

    ui->mainView->envelopes[idx]->update();
    ui->mainView->updateToolTransf();
    ui->mainView->updateBuffers();
    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_tanContCheckBox_toggled Updates the tangetial continuity settings of the envelopes.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_tanContCheckBox_toggled(bool checked){
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->envelopes[idx]->setIsTanContinuous(checked);

    ui->angleOrient_1_SpinBox->setEnabled(checked);
    ui->angleOrient_2_SpinBox->setEnabled(checked);
    ui->orientVector_1_2->setEnabled(!checked);
    ui->orientVector_2_2->setEnabled(!checked);

    ui->mainView->envelopes[idx]->update();
    ui->mainView->updateToolTransf();
    ui->mainView->updateBuffers();
    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}


/**
 * @brief MainWindow::on_angleOrient_1_SpinBox_valueChanged Updates the angle of the axis initial orientation of the
 * second tool with respect to the one of the first tool.
 * @param value new angle.
 */
void MainWindow::on_angleOrient_1_SpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    Envelope *env = ui->mainView->envelopes[idx];
    env->setAdjacentAxisAngles(value, ui->angleOrient_2_SpinBox->value());

    env->update();
    ui->mainView->updateToolTransf();
    ui->mainView->updateBuffers();
    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleOrient_2_SpinBox_valueChanged Updates the angle of the axis final orientation of the
 * second tool with respect to the one of the first tool.
 * @param value new angle.
 */
void MainWindow::on_angleOrient_2_SpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    Envelope *env = ui->mainView->envelopes[idx];
    env->setAdjacentAxisAngles(ui->angleOrient_1_SpinBox->value(), value);

    env->update();
    ui->mainView->updateToolTransf();
    ui->mainView->updateBuffers();
    ui->mainView->updateUniformsRequired = true;
    ui->mainView->update();
}







/**
 * @brief MainWindow::on_axisSectorsSpinBox_valueChanged Updates the number of sectors for the construction of the cylinder.
 * @param value The new number of sectors.
 */
void MainWindow::on_axisSectorsSpinBox_valueChanged(int value) {
    qDebug() << "Axis sectors changed";
    ui->aSlider->setMaximum(value);
    ui->mainView->cylinders[0]->setSectors(value);
    ui->mainView->envelopes[0]->setTool(ui->mainView->cylinders[0]);
    ui->mainView->cylinders[1]->setSectors(value);
    ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);

    ui->mainView->updateBuffers();
    ui->mainView->updateToolTransf();
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_timeSectorsSpinBox_valueChanged Updates the number of sectors for the construction of the path.
 * @param value The new number of sectors.
 */
void MainWindow::on_timeSectorsSpinBox_valueChanged(int value) {
  qDebug() << "Time sectors changed";
  ui->TimeSlider->setMaximum(value);
  SimplePath path = ui->mainView->movements[0]->getPath();

  path.setSectors(value);

  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radiusSpinBox_valueChanged Updates the radius of the tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_valueChanged(double value) {
  ui->mainView->cylinders[0]->setRadius(value);
  ui->mainView->drums[0]->setMidRadius(value);
  ui->radius0SpinBox->setMinimum(ui->mainView->drums[0]->getMinR0());
  switch (ui->mainView->settings.toolIdx){
  case 0:
      ui->mainView->envelopes[0]->setTool(ui->mainView->cylinders[0]);
      break;
  case 1:
      ui->mainView->envelopes[0]->setTool(ui->mainView->drums[0]);
  default:
      break;
  }

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinders[1]->setRadius(value);
    ui->radiusSpinBox_2->setValue(value);
    ui->mainView->drums[1]->setMidRadius(value);
    ui->radius0SpinBox_2->setMinimum(ui->mainView->drums[1]->getMinR0());
    switch (ui->mainView->settings.tool2Idx){
    case 0:
        ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);
        break;
    case 1:
        ui->mainView->envelopes[1]->setTool(ui->mainView->drums[1]);
    default:
        break;
    }
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
  ui->mainView->drums[0]->setRadius(value);
  ui->mainView->envelopes[0]->setTool(ui->mainView->drums[0]);

  if (ui->mainView->settings.secondEnv){
    ui->mainView->drums[1]->setRadius(value);
    ui->radius0SpinBox_2->setValue(value);
    ui->mainView->envelopes[1]->setTool(ui->mainView->drums[1]);
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
  ui->mainView->cylinders[0]->setAngle(value);
  ui->mainView->envelopes[0]->setTool(ui->mainView->cylinders[0]);

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinders[1]->setAngle(value);
    ui->angleSpinBox_2->setValue(value);
    ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);
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
  ui->mainView->cylinders[0]->setHeight(value);
  ui->mainView->drums[0]->setHeight(value);
  ui->radius0SpinBox->setMinimum(ui->mainView->drums[0]->getMinR0());

  if (ui->mainView->settings.secondEnv){
    ui->mainView->cylinders[1]->setHeight(value);
    ui->heightSpinBox_2->setValue(value);
    ui->mainView->drums[1]->setHeight(value);
    ui->radius0SpinBox_2->setMinimum(ui->mainView->drums[1]->getMinR0());

    ui->mainView->envelopes[0]->setTool(ui->mainView->cylinders[0]);
    ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);
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

  Tool *tool = nullptr;
  switch(index) {
  case 0:
      tool = ui->mainView->cylinders[0];
      break;
  case 1:
      tool = ui->mainView->drums[0];
      break;
  }

  ui->mainView->envelopes[0]->setTool(tool);
  ui->mainView->envelopes[0]->update();
  ui->mainView->toolRenderers[0]->setTool(tool);
  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radiusSpinBox_2_valueChanged Updates the radius of the second tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinders[1]->setRadius(value);
  ui->mainView->drums[1]->setMidRadius(value);
  ui->radius0SpinBox_2->setMinimum(ui->mainView->drums[1]->getMinR0());

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_radius0SpinBox_2_valueChanged Updates the inner radius of the second drum.
 * @param value new inner radius.
 */
void MainWindow::on_radius0SpinBox_2_valueChanged(double value) {
  ui->mainView->drums[1]->setRadius(value);

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_2_valueChanged Updates the opening angle of the second cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinders[1]->setAngle(value);

  ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);

  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_2_valueChanged Updates the height (length) of the second tool.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_2_valueChanged(double value) {
  ui->mainView->cylinders[1]->setHeight(value);
  ui->mainView->drums[1]->setHeight(value);
  ui->radius0SpinBox_2->setMinimum(ui->mainView->drums[1]->getMinR0());

  ui->mainView->envelopes[1]->setTool(ui->mainView->cylinders[1]);

  ui->mainView->updateToolTransf();
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

  bool success = ui->mainView->movements[1]->setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 1 vector");
  }

  ui->mainView->updateToolTransf();
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

  bool success = ui->mainView->movements[1]->setAxisDirections(vector1,vector2);
  if (success){
      ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
  } else {
      error.showMessage("The inputed vector is not a valid orientation 2 vector");
  }

  ui->mainView->updateToolTransf();
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

  Tool *tool = nullptr;
  switch(index) {
  case 0:
      tool = ui->mainView->cylinders[1];
      break;
  case 1:
      tool = ui->mainView->drums[1];
      break;
  }

  ui->mainView->envelopes[1]->setTool(tool);
  ui->mainView->envelopes[1]->update();
  ui->mainView->toolRenderers[1]->setTool(tool);

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_x_valueChanged Updates the a coefficient of the x(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_x_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
  qDebug() << "a of x(t) updated to: " << value;


  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial x = path.getX();
  x.setA(value);

  path.setX(x);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);

  ui->mainView->envelopes[idx]->update();
  ui->mainView->updateToolTransf();
  ui->mainView->updateBuffers();
  ui->mainView->updateUniformsRequired = true;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_x_valueChanged Updates the b coefficient of the x(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_x_valueChanged(int value) {
  qDebug() << "b of x(t) updated to: " << value;

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial x = path.getX();
  x.setB(value);

  path.setX(x);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial x = path.getX();
  x.setC(value);

  path.setX(x);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial y = path.getY();
  y.setA(value);

  path.setY(y);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial y = path.getY();
  y.setB(value);

  path.setY(y);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial y = path.getY();
  y.setC(value);

  path.setY(y);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial z = path.getZ();
  z.setA(value);

  path.setZ(z);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial z = path.getZ();
  z.setB(value);

  path.setZ(z);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  Polynomial z = path.getZ();
  z.setC(value);

  path.setZ(z);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);
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

  SimplePath path = ui->mainView->movements[0]->getPath();
  float sliderTimePerSector = (ui->mainView->settings.time-path.getT0()) / (path.getRange());

  path.setRange(value, path.getT1());
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);

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

  SimplePath path = ui->mainView->movements[0]->getPath();
  float sliderTimePerSector = (ui->mainView->settings.time-path.getT0()) / (path.getRange());

  path.setRange(path.getT0(), value);
  ui->mainView->movements[0]->setPath(path);
  ui->mainView->movements[1]->setPath(path);

  ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
  ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);

  ui->mainView->setTime(sliderTimePerSector*path.getRange());

  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_reflecLinesCheckBox_toggled Updates the envelope's shading.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_reflecLinesCheckBox_toggled(bool checked){
    ui->fracReflSpinBox->setEnabled(checked);
    ui->freqReflSpinBox->setEnabled(checked);

    ui->mainView->settings.reflectionLines = checked;
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

void MainWindow::on_freqReflSpinBox_valueChanged(int value){
    ui->mainView->settings.reflFreq = value;
    ui->mainView->updateBuffers();
    ui->mainView->update();
}

void MainWindow::on_fracReflSpinBox_valueChanged(double value){
    ui->mainView->settings.percentBlack = value;
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
  SimplePath path = ui->mainView->movements[0]->getPath();
  float divisor = path.getSectors() / (path.getRange());
  ui->mainView->setTime(value / divisor);
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
