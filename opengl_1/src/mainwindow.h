#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class is the main application window. Among other
 * things, it handles the inputs from the screen widgets.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  Ui::MainWindow *ui;

  explicit MainWindow(QWidget *parent = nullptr);
  void renderToFile();
  ~MainWindow() override;

 private slots:
  void on_radiusSpinBox_valueChanged(double value);
  void on_angleSpinBox_valueChanged(double value);
  void on_heightSpinBox_valueChanged(double value);
  void on_TimeSlider_sliderMoved(int value);

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

  void on_ResetRotationButton_clicked(bool checked);
  void on_RotationDialX_sliderMoved(int value);
  void on_RotationDialY_sliderMoved(int value);
  void on_RotationDialZ_sliderMoved(int value);

  void on_ResetScaleButton_clicked(bool checked);
  void on_ScaleSlider_sliderMoved(int value);
};

#endif  // MAINWINDOW_H
