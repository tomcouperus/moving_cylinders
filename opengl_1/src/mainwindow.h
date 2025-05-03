#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QComboBox>

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class is the main application window. Among other
 * things, it handles the inputs from the screen widgets.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

    QErrorMessage error;
 public:
  Ui::MainWindow *ui;

  explicit MainWindow(QWidget *parent = nullptr);
  void renderToFile();
  ~MainWindow() override;

 private:
  void SetComboBoxItemEnabled(QComboBox *comboBox, int index, bool enabled);

 private slots:

  void on_envelopeSelectBox_currentIndexChanged(int index);
  void on_envelopeActiveCheckBox_toggled(bool checked);
  void on_constraintA0SelectBox_currentIndexChanged(int index);
  void on_constraintA1SelectBox_currentIndexChanged(int index);





  void on_reflecLinesCheckBox_toggled(bool checked);
  void on_freqReflSpinBox_valueChanged(int value);
  void on_fracReflSpinBox_valueChanged(double value);


  void on_envelopeCheckBox_toggled(bool checked);
  void on_toolCheckBox_toggled(bool checked);
  void on_grazCurveCheckBox_toggled(bool checked);
  void on_pathCheckBox_toggled(bool checked);
  void on_toolAxisCheckBox_toggled(bool checked);
  void on_normalsCheckBox_toggled(bool checked);
  void on_sphereCheckBox_toggled(bool checked);

  void on_axisSectorsSpinBox_valueChanged(int value);
  void on_timeSectorsSpinBox_valueChanged(int value);

  void on_radiusSpinBox_valueChanged(double value);
  void on_radius0SpinBox_valueChanged(double value);
  void on_angleSpinBox_valueChanged(double value);
  void on_heightSpinBox_valueChanged(double value);
  void on_orientVector_1_returnPressed();
  void on_orientVector_2_returnPressed();
  void on_toolBox_currentIndexChanged(int index);

  void on_tanContCheckBox_toggled(bool checked);
  void on_positContCheckBox_toggled(bool checked);
  void on_radiusSpinBox_2_valueChanged(double value);
  void on_radius0SpinBox_2_valueChanged(double value);
  void on_angleSpinBox_2_valueChanged(double value);
  void on_heightSpinBox_2_valueChanged(double value);
  void on_orientVector_1_2_returnPressed();
  void on_orientVector_2_2_returnPressed();
  void on_toolBox_2_currentIndexChanged(int index);
  void on_angleOrient_1_SpinBox_valueChanged(double value);
  void on_angleOrient_2_SpinBox_valueChanged(double value);

  void on_spinBox_a_x_valueChanged(int value);
  void on_spinBox_b_x_valueChanged(int value);
  void on_spinBox_c_x_valueChanged(int value);
  void on_spinBox_a_y_valueChanged(int value);
  void on_spinBox_b_y_valueChanged(int value);
  void on_spinBox_c_y_valueChanged(int value);
  void on_spinBox_a_z_valueChanged(int value);
  void on_spinBox_b_z_valueChanged(int value);
  void on_spinBox_c_z_valueChanged(int value);
  void on_spinBox_t_0_valueChanged(int value);
  void on_spinBox_t_1_valueChanged(int value);

  void on_TimeSlider_sliderMoved(int value);
  void on_aSlider_sliderMoved(int value);

  void on_ResetRotationButton_clicked(bool checked);
  void on_RotationDialX_sliderMoved(int value);
  void on_RotationDialY_sliderMoved(int value);
  void on_RotationDialZ_sliderMoved(int value);

  void on_ResetScaleButton_clicked(bool checked);
  void on_ScaleSlider_sliderMoved(int value);
};

#endif  // MAINWINDOW_H
