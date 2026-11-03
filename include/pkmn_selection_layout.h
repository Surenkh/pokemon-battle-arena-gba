#pragma once
namespace pkmn {
inline constexpr int selection_columns=5;
inline constexpr int selection_grid_x=8;
inline constexpr int selection_grid_y=20;
inline constexpr int selection_step_x=45;
inline constexpr int selection_step_y=28;
inline constexpr int selection_card_width=44;
inline constexpr int selection_card_height=26;
inline constexpr int selection_portrait_x=22;
inline constexpr int selection_portrait_y=8;
inline constexpr int selection_quick_rows[4]={0,114,128,141};
inline constexpr int selection_practice_rows[5]={0,108,120,132,144};
inline constexpr int selection_footer_y=155;
inline constexpr int selection_preview_x=188;
inline constexpr int selection_preview_y=88;
inline int selection_row_y(bool training,int row){return training?selection_practice_rows[row]:selection_quick_rows[row];}
}
