use core::fmt;
use std::{
    collections::VecDeque,
    default,
    fmt::{Display, Formatter},
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

#[derive(Debug, PartialEq, Default)]
pub struct UIBox {
    pub hash_next: Option<Box<UIBox>>,
    pub hash_prev: Option<Box<UIBox>>,

    // Per build links/data
    pub first_child: Option<Box<UIBox>>,
    pub last_child: Option<Box<UIBox>>,
    pub next_sibling: Option<Box<UIBox>>,
    pub prev_sibling: Option<Box<UIBox>>,
    pub child_count: u64,

    // Per build
    pub flags: UIBoxFlag,
    pub fixed_pos: Vec2,
    pub fixed_size: Vec2,
    // pub x_axis: UISize,
    // pub y_axis: UISize,

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
