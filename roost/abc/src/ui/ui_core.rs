use core::fmt;
use std::{
    collections::VecDeque,
    default,
    fmt::{Display, Formatter},
    rc::Rc,
};

use bitflags::bitflags;
use core::fmt::Debug;
use glam::Vec2;

const UI_BOX_FLAGS: u32 = 1;

bitflags! {
    pub struct UIBoxFlag: u32 {
        const UI_BoxFlag_Clickable = UI_BOX_FLAGS << 0;
    }

}

impl Clone for UIBoxFlag {
    fn clone(&self) -> Self {
        Self(self.0.clone())
    }
}

impl Display for UIBoxFlag {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{:032b}", self.bits())
    }
}

impl Debug for UIBoxFlag {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{:032b}", self.bits())
    }
}

impl PartialEq for UIBoxFlag {
    fn eq(&self, other: &Self) -> bool {
        self.0 == other.0
    }
}

impl Default for UIBoxFlag {
    fn default() -> Self {
        Self(Default::default())
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Default)]
pub enum SemanticSize {
    #[default]
    Nil,
    Pixels(f32),          // Fixed pixel size
    TextContent(f32),     // Size based on content
    PercentOfParent(f32), // Size as percentage of parent
    ChildrenSum,          // Size is the sum of children
    BiggestChild,         // Size is based on the largest child
}

#[derive(Debug, Clone, Copy, PartialEq, Default)]
pub enum Axis {
    #[default]
    X,
    Y,
}

#[derive(Debug, Clone, Copy, PartialEq, Default)]
pub struct UISize {
    pub size_type: SemanticSize,
    pub strictness: f32, // How much size can be given up if it exceeds limits (eg. overflow)
}

#[derive(Debug, PartialEq, Default, Clone)]
pub struct UIBox {
    // pub hash_next: Option<Box<UIBox>>,
    // pub hash_prev: Option<Box<UIBox>>,

    // Per build links/data
    pub first: Option<Rc<UIBox>>,
    pub last: Option<Rc<UIBox>>,
    pub next: Option<Rc<UIBox>>,
    pub prev: Option<Rc<UIBox>>,
    pub child_count: u64,

    // Per build
    pub flags: UIBoxFlag,
    pub fixed_pos: Vec2,
    pub fixed_size: Vec2,
    pub x_axis: UISize,
    pub y_axis: UISize,
    pub child_layout_axis: Axis,

    // persistent data
    pub hot: bool,
    pub active: bool,
}

#[derive(Default)]
pub struct UIState {
    pub root: Option<Box<UIBox>>,

    pref_width_stack: VecDeque<UISize>,
    pref_height_stack: VecDeque<UISize>,
}

impl UIState {
    pub fn calculate_layout(&mut self, box_node: &mut Rc<UIBox>, parent_size: Vec2) {
        // Set size based on its type
        box_node.fixed_size = Vec2::new(
            match box_node.x_axis.size_type {
                SemanticSize::Pixels(pixels) => pixels,
                SemanticSize::PercentOfParent(percent) => parent_size.x * percent,
                _ => parent_size.x, // Fallback to parent's size for simplicity
            },
            match box_node.y_axis.size_type {
                SemanticSize::Pixels(pixels) => pixels,
                SemanticSize::PercentOfParent(percent) => parent_size.y * percent,
                _ => parent_size.y, // Fallback to parent's size for simplicity
            },
        );

        // Calculate positions for children based on the layout axis
        if box_node.child_layout_axis == Axis::X {
            let child_count = box_node.child_count;
            let child_width = box_node.fixed_size.x / child_count as f32;

            let mut child = box_node.first.clone();
            let mut offset_x = 0.0;

            while let Some(child_panel) = child {
                child_panel.fixed_pos = Vec2::new(offset_x, 0.0);
                offset_x += child_width; // Move to the next position
                child = child_panel.next.clone(); // Move to the next sibling
            }
        } else {
            // Handle Y-axis layout if needed
            let child_count = box_node.child_count;
            let child_height = box_node.fixed_size.y / child_count as f32;

            let mut child = &mut box_node.first;
            let mut offset_y = 0.0;

            while let Some(child_panel) = child {
                child_panel.fixed_pos = Vec2::new(0.0, offset_y);
                offset_y += child_height; // Move to the next position
                child = &mut child_panel.next; // Move to the next sibling
            }
        }

        // Recursively calculate for children
        if let Some(ref mut first) = box_node.first {
            self.calculate_layout(first, box_node.fixed_size);
        }
    }
}
