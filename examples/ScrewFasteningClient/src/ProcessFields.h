/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ProcessFields.h
 * @brief 检查工艺页面允许使用的参数名称。
 */

#pragma once

#include <string>

namespace screw {

/**
 * @brief 检查参数是否属于指定工艺，未知工艺或参数返回 false。
 * @note 客户端与控制器必须使用完全一致的参数表，增删参数时同步更新两个仓库。
 */
inline bool processFieldAllowed(const std::string &type, const std::string &field)
{
    const char *const fields[][17] = {
        {"TorqueSetting", "tooth_feed_speed", "tooth_feed_angle", "tighten_speed", "tighten_angle",
         "approach_speed", "initial_torque", "target_torque", "float_lock_angle", "strip_angle",
         "reverse_angle", "reverse_speed", "torque_compensation", "force_speed_ratio",
         "self_tapping_mode", "coefficient_correction", nullptr},
        {"SuctionProcess", "x_axis_pos", "x_axis_pos_3rd", "safe_height", "pick_time",
         "follow_height", "follow_speed", "material_signal_in", "align_signal_out", nullptr},
        {"BlowProcess", "blow_safe_height", "blow_arrive_delay", "blow_lift_height", nullptr},
        {"ScrewPoint", "screw_x_axis_pos", "screw_x_axis_pos_3rd", "screw_index",
         "screw_position_index", "screw_relock", nullptr},
        {"IoOutput", "io_out_port", "io_out_state", nullptr},
        {"IoWait", "io_wait_input", "io_wait_state", "io_wait_timeout", nullptr},
    };
    for (const auto &process : fields) {
        if (type != process[0])
            continue;
        for (int index = 1; process[index] != nullptr; ++index) {
            if (field == process[index])
                return true;
        }
        return false;
    }
    return false;
}

} // namespace screw
