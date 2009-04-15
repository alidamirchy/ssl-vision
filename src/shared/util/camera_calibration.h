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
  \file    camera_calibration.h
  \brief   Datatypes for camera parameters, functions for perspective transformations
  \author  Tim Laue, (C) 2009
*/
//========================================================================

#include <VarDouble.h>
#include <VarList.h>
#include <quaternion.h>
#include <Eigen/Core>
#include "field.h"
#include "timer.h"

#ifndef CAMERA_CALIBRATION_H
#define CAMERA_CALIBRATION_H
#ifndef NO_PROTOBUFFERS
  #include "messages_robocup_ssl_geometry.pb.h"
#endif


//using namespace Eigen;
//USING_PART_OF_NAMESPACE_EIGEN

/*!
  \class CameraParameters

  \brief Intrinsic and extrinsic camera parameters.

  \author Tim Laue, (C) 2009
**/
class CameraParameters
{
public:
  
  class AdditionalCalibrationInformation;
  class LSCalibrationData;

  CameraParameters(RoboCupCalibrationHalfField &field);
  ~CameraParameters();
  void addSettingsToList(VarList& list);
  
  VarDouble* focal_length;
  VarDouble* principal_point_x;
  VarDouble* principal_point_y;
  VarDouble* distortion;

  // Just for automatic saving purposes
  VarDouble* q0;
  VarDouble* q1;
  VarDouble* q2;
  VarDouble* q3;

  VarDouble* tx;
  VarDouble* ty;
  VarDouble* tz;

  std::vector<LSCalibrationData> line_segment_data;

  Eigen::VectorXd p_alpha;
  Quaternion<double> q_rotate180;

  //Quaternion<double> q_field2cam;
  //GVector::vector3d<double> translation;  
  
  AdditionalCalibrationInformation* additional_calibration_information;
  
  void field2image(const GVector::vector3d<double> &p_f, GVector::vector2d<double> &p_i) const;
  void image2field(GVector::vector3d<double> &p_f, GVector::vector2d<double> &p_i, double z) const;
  void calibrate(std::vector<GVector::vector3d<double> > &p_f, std::vector<GVector::vector2d<double> > &p_i, int cal_type);
  
  double calc_chisqr(std::vector<GVector::vector3d<double> > &p_f, std::vector<GVector::vector2d<double> > &p_i, Eigen::VectorXd &p, int);
  void field2image(GVector::vector3d<double> &p_f, GVector::vector2d<double> &p_i, Eigen::VectorXd &p);

  #ifndef NO_PROTOBUFFERS
  void toProtoBuffer(SSL_GeometryCameraCalibration & buffer, int camera_id) const {
    buffer.set_focal_length(focal_length->getDouble());
    buffer.set_principal_point_x(principal_point_x->getDouble());
    buffer.set_principal_point_y(principal_point_y->getDouble());
    buffer.set_distortion(distortion->getDouble());
    buffer.set_q0(q0->getDouble());
    buffer.set_q1(q1->getDouble());
    buffer.set_q2(q2->getDouble());
    buffer.set_q3(q3->getDouble());
    buffer.set_tx(tx->getDouble());
    buffer.set_ty(ty->getDouble());
    buffer.set_tz(tz->getDouble());
    buffer.set_camera_id(camera_id);
  }

  void fromProtoBuffer(const SSL_GeometryCameraCalibration & buffer) {
    focal_length->setDouble(buffer.focal_length());
    principal_point_x->setDouble(buffer.principal_point_x());
    principal_point_y->setDouble(buffer.principal_point_y());
    distortion->setDouble(buffer.distortion());
    q0->setDouble(buffer.q0());
    q1->setDouble(buffer.q1());
    q2->setDouble(buffer.q2());
    q3->setDouble(buffer.q3());
    tx->setDouble(buffer.tx());
    ty->setDouble(buffer.ty());
    tz->setDouble(buffer.tz());
  }
  #endif

  enum 
  {
    FOCAL_LENGTH=0,
    PP_X,
    PP_Y,
    DIST,
    Q_1,
    Q_2,
    Q_3,
    T_1,
    T_2,
    T_3,
    STATE_SPACE_DIMENSION
  };

  enum
  {
    FOUR_POINT_INITIAL = 1,
    FULL_ESTIMATION = 2
    // ... = 4 etc.
  };

  std::vector<int> p_to_est;
  
  const RoboCupCalibrationHalfField & field;
  /*!
  \class AdditionalCalibrationInformation
  \brief Some additional data used for calibration 
  \author Tim Laue, (C) 2009
   **/
  class AdditionalCalibrationInformation
  {
    public:
      AdditionalCalibrationInformation();
      ~AdditionalCalibrationInformation();
      void addSettingsToList(VarList& list);  
  
      VarDouble* left_corner_image_x;
      VarDouble* left_corner_image_y;
     /*  VarDouble* left_goal_area_image_x; */
/*       VarDouble* left_goal_area_image_y; */
/*       VarDouble* right_goal_area_image_x; */
/*       VarDouble* right_goal_area_image_y; */
      VarDouble* right_corner_image_x;
      VarDouble* right_corner_image_y;
      VarDouble* left_centerline_image_x;
      VarDouble* left_centerline_image_y;
    /*   VarDouble* left_centercircle_image_x; */
/*       VarDouble* left_centercircle_image_y; */
/*       VarDouble* centerpoint_image_x; */
/*       VarDouble* centerpoint_image_y; */
/*       VarDouble* right_centercircle_image_x; */
/*       VarDouble* right_centercircle_image_y; */
      VarDouble* right_centerline_image_x;
      VarDouble* right_centerline_image_y;
      VarDouble* initial_distortion;
      VarDouble* camera_height;
      VarDouble* line_search_corridor_width;
      VarDouble* convergence_timeout;
      VarDouble* cov_corner_x;
      VarDouble* cov_corner_y;
      
      VarDouble* cov_ls_x;
      VarDouble* cov_ls_y;
  };
  
  /*!
  \class LSCalibrationData
  \brief Additional structure for holding information about 
  image points on line segments
  \author OB, (C) 2009
   **/
  class LSCalibrationData
  {
    public:
    GVector::vector3d<double> p1;
    GVector::vector3d<double> p2;
    std::vector< std::pair<GVector::vector2d<double>,bool> > pts_on_line;
    bool horizontal;
  };
  

public:
  void do_calibration(int cal_type);
  void reset();
};

#endif