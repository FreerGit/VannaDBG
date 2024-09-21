use core::fmt;
use std::{
    collections::VecDeque,
    default,
    fmt::{Display, Formatter},
};

use bitflags::bitflags;
use glam::Vec2;

const UI_BOX_FLAGS: u32 = 1;

bitflags! {
    pub struct UIBoxFlag: u32 {
        const UI_BoxFlag_Clickable = UI_BOX_FLAGS << 0;
    }

}

impl Display for UIBoxFlag {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{:032b}", self.bits())
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum SemanticSize {
    Pixels(f32),          // Fixed pixel size
    TextContent(f32),     // Size based on content
    PercentOfParent(f32), // Size as percentage of parent
    ChildrenSum,          // Size is the sum of children
    BiggestChild,         // Size is based on the largest child
}

#[derive(Default)]
pub enum Axis {
    #[default]
    X,
    Y,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct UISize {
    pub size_type: SemanticSize,
    pub strictness: f32, // How much size can be given up if it exceeds limits (eg. overflow)
}

pub struct UIBox {
    pub hash_next: Option<Box<UIBox>>,
    pub hash_prev: Option<Box<UIBox>>,

    // Per build links/data
    pub first_child: Option<Box<UIBox>>,
    pub last_child: Option<Box<UIBox>>,
    pub next_sibling: Option<Box<UIBox>>,
    pub prev_sibling: Option<Box<UIBox>>,
    pub child_count: u64,

    pub position: Vec2,
    pub size: Vec2,
    pub flags: UIBoxFlag,

    // Per build
    pub fixed_pos: Vec2,
    pub fixed_size: Vec2,
    pub x_axis: UISize,
    pub y_axis: UISize,

    // persistent data
    pub hot: bool,
    pub active: bool,
}

#[derive(Default)]
pub struct UIState {
    root: Option<Box<UIBox>>,

    pref_width_stack: VecDeque<UISize>,
    pref_height_stack: VecDeque<UISize>,
}

// impl UIBox {
//     pub fn add_child(&mut self, mut child: UIBox) {
//         // Retrieve the preferred width and height from the UIState stacks
//         let child_pref_width = ui_state.pref_width_stack.top.map_or(self.pref_width_stack.bottom_val, |node| node.value);
//         let child_pref_height = self.pref_height_stack.top.map_or(self.pref_height_stack.bottom_val, |node| node.value);

//         // Assign the preferred sizes to the child box, replacing direct width/height
//         child.set_pref_size(child_pref_width, child_pref_height);

//         // Handle linking child into the sibling list
//         match self.first_child.take() {
//             Some(mut first) => {
//                 first.prev_sibling = Some(Box::new(child));
//                 self.first_child = Some(Box::new(first));
//             },
//             None => {
//                 self.first_child = Some(Box::new(child));
//             }
//         }

//         if self.last_child.is_none() {
//             self.last_child = self.first_child.clone();
//         }
//     }
//     pub fn set_pref_size(&mut self, width: UI_Size, height: UI_Size) {
//         // Set the preferred width and height using the stack values
//         self.pref_width = width;
//         self.pref_height = height;
//     }

//     pub fn layout(&mut self) {
//         // Iterate through all children and apply layout based on pref sizes
//         let mut current_child = self.first_child.as_mut();

//         while let Some(child) = current_child {
//             // Get preferred size from the pref_width_stack and pref_height_stack
//             let pref_width = child.pref_width_stack.top.map_or(child.pref_width_stack.bottom_val, |node| node.value);
//             let pref_height = child.pref_height_stack.top.map_or(child.pref_height_stack.bottom_val, |node| node.value);

//             // Apply size to child and adjust layout based on the preferred size
//             child.width = pref_width;
//             child.height = pref_height;

//             // Layout logic for positioning children here

//             current_child = child.next_sibling.as_mut();
//         }
//     }

//     fn ui_divider(ui_state: &mut UIState, size: UI_Size) {
//         let parent = ui_state.top_parent();

//         // Set the preferred size of the next child to the parent's layout axis size
//         ui_state.set_next_pref_size(parent.child_layout_axis, size);

//         // Set the child layout axis for the next element
//         ui_state.set_next_child_layout_axis(parent.child_layout_axis);

//         // Build the UI_Box, inheriting the parent's layout axis and preferred size
//         let box = ui_state.build_box_from_key(0, ui_state.key_zero());

//         // Set the preferred size to fill parent in the child layout axis
//         ui_state.parent(box);
//         ui_state.set_pref_size(parent.child_layout_axis, ui_pct(1, 0));

//         // Build nested boxes inside the parent
//         ui_state.build_box_from_key(UI_BoxFlag_DrawSideBottom, ui_state.key_zero());
//         ui_state.build_box_from_key(0, ui_state.key_zero());
//     }
// }

impl UIState {
    // pub fn calculate_standalone_sizes(&mut self) {
    //     // Start with the root UI_Box (or top-level box)
    //     let mut current_box = self.root.as_mut();

    //     // Iterate through all the boxes in the UI tree
    //     while let Some(ui_box) = current_box {
    //         // Retrieve preferred width and height from the pref_width_stack and pref_height_stack
    //         let pref_width = self.pref_width_stack.back()
    //             .map_or(self.pref_width_stack.bottom_val, |node| node.value);

    //         let pref_height = self.pref_height_stack.top
    //             .map_or(self.pref_height_stack.bottom_val, |node| node.value);

    //         // Assign these preferred sizes to the current box's standalone size
    //         ui_box.standalone_width = pref_width;
    //         ui_box.standalone_height = pref_height;

    //         // Move to the next box (could be a depth-first traversal or whatever layout method you're using)
    //         current_box = box.next_sibling.as_mut();
    //     }
    // }
}
