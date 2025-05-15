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

QString MainWindow::QVectorToString(const QVector3D &v) {
    return QString("(%1,%2,%3)").arg(v.x()).arg(v.y()).arg(v.z());
}

/***********************************************************/
/********************** Envelope Menu **********************/
/***********************************************************/

void MainWindow::on_envelopeSelectBox_currentIndexChanged(int index) {
    int idx = index-1;
    int prevIdx = ui->mainView->settings.selectedIdx;
    qDebug() << "Selected envelope" << idx;
    ui->mainView->settings.selectedIdx = idx;
    ui->mainView->settings.prevIdx = prevIdx;

    // Unhide options from constraint selectors
    QVector<Envelope *> envelopes = ui->mainView->envelopes;
    if (prevIdx != -1) {
        SetComboBoxItemEnabled(ui->constraintA0SelectBox, prevIdx+1, true);
        SetComboBoxItemEnabled(ui->constraintA1SelectBox, prevIdx+1, true);
        if (envelopes[prevIdx]->isPositContinuous()) {
            SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelopes[prevIdx]->getAdjEnvelope()->getIndex()+1, true);
        }
    }

    if (idx == -1) { // No Envelope selected
        // Reset the ui elements
        // ** Active check box
        ui->envelopeActiveCheckBox->setChecked(false);
        // ** Constraint select boxes. Also need to unhide all the currently hidden options,
        // ** which always has to happen on a switch. So do that outside this if.
        ui->constraintA0SelectBox->setCurrentIndex(0);
        ui->constraintA1SelectBox->setCurrentIndex(0);
        ui->SettingsTabMenu->setTabEnabled(3, false);
    } else { // Envelope Selected
        Envelope *env = ui->mainView->envelopes[idx];
        // Load the selected envelope's data
        // ** Active check box
        ui->envelopeActiveCheckBox->setChecked(env->isActive());
        // ** Constraint select boxes. Also hide the current envelope and all its dependents
        QList<int> dependents = envelopes[idx]->getAllDependents().values();
        for (int i = 0; i < dependents.size(); i++) {
            SetComboBoxItemEnabled(ui->constraintA0SelectBox, dependents[i]+1, false);
            SetComboBoxItemEnabled(ui->constraintA1SelectBox, dependents[i]+1, false);
        }
        int a0Idx = env->isPositContinuous() ? env->getAdjEnvelope()->getIndex()+1 : 0;
        ui->constraintA0SelectBox->setCurrentIndex(a0Idx);
        on_constraintA0SelectBox_currentIndexChanged(a0Idx);
        ui->tanContCheckBox->setChecked(env->isTanContinuous());
        on_tanContCheckBox_toggled(env->isTanContinuous());

        // Tool settings
        ui->orientVector_1->setText(QVectorToString(env->getToolMovement().getAxisT0()));
        ui->orientVector_2->setText(QVectorToString(env->getToolMovement().getAxisT1()));
        ui->angleOrient_1_SpinBox->setValue(env->getAdjAxisAngle1());
        ui->angleOrient_2_SpinBox->setValue(env->getAdjAxisAngle2());

        Cylinder* cyl = ui->mainView->cylinders[idx];
        Drum* drum = ui->mainView->drums[idx];
        ui->radiusSpinBox->setValue(cyl->getRadius());
        ui->drumRadiusSpinBox->setValue(drum->getRadiusCurvature());
        ui->angleSpinBox->setValue(cyl->getAngle());
        ui->heightSpinBox->setValue(cyl->getHeight());
        int toolTypeIdx = (int) env->getTool()->getType();
        ui->toolBox->setCurrentIndex(toolTypeIdx);

        // Path Settings
        SimplePath &path = env->getToolMovement().getPath();
        ui->spinBox_a_x->setValue(path.getX().getA());
        ui->spinBox_b_x->setValue(path.getX().getB());
        ui->spinBox_c_x->setValue(path.getX().getC());
        ui->spinBox_d_x->setValue(path.getX().getD());
        ui->spinBox_a_y->setValue(path.getY().getA());
        ui->spinBox_b_y->setValue(path.getY().getB());
        ui->spinBox_c_y->setValue(path.getY().getC());
        ui->spinBox_d_y->setValue(path.getY().getD());
        ui->spinBox_a_z->setValue(path.getZ().getA());
        ui->spinBox_b_z->setValue(path.getZ().getB());
        ui->spinBox_c_z->setValue(path.getZ().getC());
        ui->spinBox_d_z->setValue(path.getZ().getD());
    }

    // Enable/disable some ui elements always
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
        // TODO dependency check
        QSet<int> depEnvs = envelope->getAllDependents();
        ui->mainView->envelopeMeshUpdates += depEnvs;
        ui->mainView->toolTransfUpdates += depEnvs;
        ui->mainView->update();
    }

}

void MainWindow::on_constraintA1SelectBox_currentIndexChanged(int index) {

}

/**
 * @brief MainWindow::on_tanContCheckBox_toggled Updates the tangetial continuity settings of the envelopes.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_tanContCheckBox_toggled(bool checked){
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->envelopes[idx]->setIsTanContinuous(checked);

    ui->orientVector_1->setEnabled(!checked);
    ui->orientVector_2->setEnabled(!checked);
    ui->angleOrient_1_SpinBox->setEnabled(checked);
    ui->angleOrient_2_SpinBox->setEnabled(checked);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

void MainWindow::on_newEnvelopeButton_clicked() {
    Envelope *env = ui->mainView->addNewEnvelope();
    int idx = env->getIndex();
    QString name = "Envelope "+QString::number(idx);
    ui->envelopeSelectBox->addItem(name);
    ui->constraintA0SelectBox->addItem(name);
    ui->constraintA1SelectBox->addItem(name);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/***********************************************************/
/************************ Tool Menu ************************/
/***********************************************************/

/**
 * @brief MainWindow::on_orientVector_1_returnPressed Updates the orientation vector of the tool.
 */
void MainWindow::on_orientVector_1_returnPressed(){
    int idx = ui->mainView->settings.selectedIdx;
    qDebug() << "orientation vector changed";
    QVector3D vector1 = ui->mainView->settings.stringToVector3D(ui->orientVector_1->text());
    QVector3D vector2 = ui->mainView->settings.stringToVector3D(ui->orientVector_2->text());

    qDebug() << "to" << vector1 << "and" << vector2;

    Envelope *env = ui->mainView->envelopes[idx];
    bool success = env->setAxes(vector1,vector2);
    if (!success){
        error.showMessage("The inputed vector is not a valid orientation 1 vector");
    }

    QSet<int> depEnvs = env->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
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

    Envelope *env = ui->mainView->envelopes[idx];
    bool success = env->setAxes(vector1,vector2);
    if (!success){
        error.showMessage("The inputed vector is not a valid orientation 2 vector");
    }

    QSet<int> depEnvs = env->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
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

    QSet<int> depEnvs = env->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
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

    QSet<int> depEnvs = env->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}


/**
 * @brief MainWindow::on_radiusSpinBox_valueChanged Updates the radius of the tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->cylinders[idx]->setRadius(value);
    ui->mainView->drums[idx]->setMidRadius(value);
    ui->drumRadiusSpinBox->setMinimum(ui->mainView->drums[idx]->getMinR0());

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_drumRadiusSpinBox_valueChanged Updates the inner radius of the drum.
 * @param value new inner radius.
 */
void MainWindow::on_drumRadiusSpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->drums[idx]->setRadius(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_valueChanged Updates the opening angle of the cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->cylinders[idx]->setAngle(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_valueChanged Updates the height (length) of the tool.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_valueChanged(double value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;
    ui->mainView->cylinders[idx]->setHeight(value);
    ui->mainView->drums[idx]->setHeight(value);
    ui->drumRadiusSpinBox->setMinimum(ui->mainView->drums[idx]->getMinR0()); // TODO why?

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolBox_currentIndexChanged Updates the tool type. Activates/Deactivates their corresponding input fields.
 * @param index The index of the tool.
 */
void MainWindow::on_toolBox_currentIndexChanged(int index){
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    qDebug() << "tool index" << index;
    switch (index) {
    case 0:
        ui->angleSpinBox->setEnabled(true);
        ui->drumRadiusSpinBox->setEnabled(false);
        break;
    case 1:
        ui->angleSpinBox->setEnabled(false);
        ui->drumRadiusSpinBox->setEnabled(true);
        break;
    default:
        ui->angleSpinBox->setEnabled(false);
        ui->drumRadiusSpinBox->setEnabled(false);
        break;
    }

    Tool *tool = nullptr;
    switch(index) {
    case 0:
        tool = ui->mainView->cylinders[idx];
        break;
    case 1:
        tool = ui->mainView->drums[idx];
        break;
    }

    ui->mainView->envelopes[idx]->setTool(tool);
    ui->mainView->toolRenderers[idx]->setTool(tool);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}


/***********************************************************/
/************************ Path Menu ************************/
/***********************************************************/

/**
 * @brief MainWindow::on_spinBox_a_x_valueChanged Updates the a coefficient of the x(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_x_valueChanged(int value) {
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;

  Envelope *env = ui->mainView->envelopes[idx];
  env->getToolMovement().getPath().getX().setA(value);

  QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  ui->mainView->toolMeshUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += depEnvs;
  ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_x_valueChanged Updates the b coefficient of the x(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_x_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getX().setB(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_x_valueChanged Updates the c coefficient of the x(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_x_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getX().setC(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_d_x_valueChanged Updates the d coefficient of the x(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_d_x_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getX().setD(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_a_y_valueChanged Updates the a coefficient of the y(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_y_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getY().setA(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_y_valueChanged Updates the b coefficient of the y(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_y_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getY().setB(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_y_valueChanged Updates the c coefficient of the y(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_y_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getY().setC(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_d_y_valueChanged Updates the d coefficient of the y(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_d_y_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getY().setD(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}


/**
 * @brief MainWindow::on_spinBox_a_z_valueChanged Updates the a coefficient of the z(t) polynomial.
 * @param value new a coefficient.
 */
void MainWindow::on_spinBox_a_z_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getZ().setA(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_b_z_valueChanged Updates the b coefficient of the z(t) polynomial.
 * @param value new b coefficient.
 */
void MainWindow::on_spinBox_b_z_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getZ().setB(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_c_z_valueChanged Updates the c coefficient of the z(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_c_z_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getZ().setC(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}

/**
 * @brief MainWindow::on_spinBox_d_z_valueChanged Updates the d coefficient of the z(t) polynomial.
 * @param value new c coefficient.
 */
void MainWindow::on_spinBox_d_z_valueChanged(int value) {
    int idx = ui->mainView->settings.selectedIdx;
    if (idx == -1) return;

    Envelope *env = ui->mainView->envelopes[idx];
    env->getToolMovement().getPath().getZ().setD(value);

    QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
    ui->mainView->toolMeshUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->update();
}



/***********************************************************/
/*********************** Render Menu ***********************/
/***********************************************************/

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
    SimplePath &path = ui->mainView->envelopes[0]->getToolMovement().getPath();

    path.setSectors(value);

    // ui->mainView->movements[0]->setPath(path);
    // ui->mainView->movements[1]->setPath(path);

    // ui->mainView->envelopes[0]->setToolMovement(ui->mainView->movements[0]);
    // ui->mainView->envelopes[1]->setToolMovement(ui->mainView->movements[1]);

    ui->mainView->updateBuffers();
    ui->mainView->updateToolTransf();
    ui->mainView->update();
}



/***********************************************************/
/******************** General Side Menu ********************/
/***********************************************************/

/**
 * @brief MainWindow::on_TimeSlider_sliderMoved Updates the time value.
 * @param value The new time value.
 */
void MainWindow::on_TimeSlider_sliderMoved(int value) {
  CylinderMovement &move = ui->mainView->envelopes[0]->getToolMovement();
  SimplePath &path = move.getPath();
  ui->mainView->settings.timeIdx = value;

  ui->mainView->updateToolTransf();
  ui->mainView->updateAllUniforms = true;
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_aSlider_sliderMoved Updates the time value for the tool axis.
 * @param value The new a value.
 */
void MainWindow::on_aSlider_sliderMoved(int value) {
  ui->mainView->settings.aIdx = value;
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_ResetRotationButton_clicked Resets the rotation.
 */
void MainWindow::on_ResetRotationButton_clicked() {
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
 */
void MainWindow::on_ResetScaleButton_clicked() {
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
