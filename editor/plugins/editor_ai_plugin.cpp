#include "editor_ai_plugin.h"
#include "../../core/config/project_settings.h"
#include "../../core/input/shortcut.h"
#include "../../scene/gui/label.h"
#include "../../scene/gui/margin_container.h"
#include "../../scene/main/node.h"
#include "../../scene/main/scene_tree.h"
#include "../../scene/resources/packed_scene.h"
#include "../editor_command_palette.h"
#include "../editor_dock_manager.h"
#include "../editor_node.h"
#include "../editor_settings.h"
#include "../editor_string_names.h"
#include "../themes/editor_scale.h"

void EditorAIPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_submit_pressed"), &EditorAIPlugin::_on_submit_pressed);
	ClassDB::bind_method(D_METHOD("_create_new_scene"), &EditorAIPlugin::_create_new_scene);
	ClassDB::bind_method(D_METHOD("_add_node_to_scene", "type", "name"), &EditorAIPlugin::_add_node_to_scene);
	ClassDB::bind_method(D_METHOD("_set_node_property", "node", "property", "value"), &EditorAIPlugin::_set_node_property);
	ClassDB::bind_method(D_METHOD("_save_current_scene"), &EditorAIPlugin::_save_current_scene);
}

const Ref<Texture2D> EditorAIPlugin::get_plugin_icon() const {
	return EditorNode::get_singleton()->get_gui_base()->get_theme_icon(SNAME("Node"), SNAME("EditorIcons"));
}

EditorAIPlugin::EditorAIPlugin() {
	// Create main container
	main_container = memnew(VBoxContainer);
	main_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->add_theme_constant_override("separation", 0);

	// Create title bar
	HBoxContainer *title_hbox = memnew(HBoxContainer);
	title_hbox->add_theme_constant_override("separation", 8 * EDSCALE);
	title_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->add_child(title_hbox);

	// Add icon
	TextureRect *icon = memnew(TextureRect);
	icon->set_texture(get_plugin_icon());
	icon->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
	icon->set_custom_minimum_size(Size2(16 * EDSCALE, 16 * EDSCALE));
	title_hbox->add_child(icon);

	// Add title label
	Label *title = memnew(Label);
	title->set_text("AI Assistant");
	title->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	title->add_theme_font_size_override("font_size", 16 * EDSCALE);
	title_hbox->add_child(title);

	// Add separator
	HSeparator *separator = memnew(HSeparator);
	separator->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->add_child(separator);

	// Create response output (top section)
	MarginContainer *response_margin = memnew(MarginContainer);
	response_margin->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	response_margin->add_theme_constant_override("margin_left", 4 * EDSCALE);
	response_margin->add_theme_constant_override("margin_right", 4 * EDSCALE);
	response_margin->add_theme_constant_override("margin_top", 4 * EDSCALE);
	response_margin->add_theme_constant_override("margin_bottom", 4 * EDSCALE);
	main_container->add_child(response_margin);

	response_output = memnew(TextEdit);
	response_output->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	response_output->set_editable(false);
	response_output->set_custom_minimum_size(Size2(0, 200 * EDSCALE));
	response_output->add_theme_constant_override("line_spacing", 4 * EDSCALE);
	response_margin->add_child(response_output);

	// Create bottom panel for prompt input
	PanelContainer *bottom_panel = memnew(PanelContainer);
	bottom_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	bottom_panel->add_theme_style_override("panel", EditorNode::get_singleton()->get_gui_base()->get_theme_stylebox(SNAME("panel"), SNAME("Panel")));
	main_container->add_child(bottom_panel);

	MarginContainer *prompt_margin = memnew(MarginContainer);
	prompt_margin->add_theme_constant_override("margin_left", 4 * EDSCALE);
	prompt_margin->add_theme_constant_override("margin_right", 4 * EDSCALE);
	prompt_margin->add_theme_constant_override("margin_top", 4 * EDSCALE);
	prompt_margin->add_theme_constant_override("margin_bottom", 4 * EDSCALE);
	bottom_panel->add_child(prompt_margin);

	VBoxContainer *prompt_vbox = memnew(VBoxContainer);
	prompt_vbox->add_theme_constant_override("separation", 4 * EDSCALE);
	prompt_margin->add_child(prompt_vbox);

	prompt_input = memnew(TextEdit);
	prompt_input->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	prompt_input->set_custom_minimum_size(Size2(0, 60 * EDSCALE));
	prompt_input->set_line_wrapping_mode(TextEdit::LineWrappingMode::LINE_WRAPPING_BOUNDARY);
	prompt_input->add_theme_constant_override("line_spacing", 4 * EDSCALE);
	prompt_vbox->add_child(prompt_input);

	HBoxContainer *button_hbox = memnew(HBoxContainer);
	button_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	prompt_vbox->add_child(button_hbox);

	submit_button = memnew(Button);
	submit_button->set_text("Submit");
	submit_button->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	submit_button->connect("pressed", callable_mp(this, &EditorAIPlugin::_on_submit_pressed));
	button_hbox->add_child(submit_button);

	// Initialize editor interface
	editor_interface = EditorInterface::get_singleton();

	// Add the dock to the editor
	EditorDockManager::get_singleton()->add_dock(main_container, TTRC("AI Assistant"), EditorDockManager::DOCK_SLOT_RIGHT_UL, ED_SHORTCUT_AND_COMMAND("docks/open_ai_assistant", TTRC("Open AI Assistant Dock")), "Node");
}

EditorAIPlugin::~EditorAIPlugin() {
	if (main_container) {
		memdelete(main_container);
	}
}

void EditorAIPlugin::make_visible(bool p_visible) {
	if (main_container) {
		main_container->set_visible(p_visible);
	}
}

void EditorAIPlugin::_on_submit_pressed() {
	String prompt = prompt_input->get_line(0); // Get first line of text
	if (!prompt.is_empty()) {
		_process_ai_request(prompt);
		prompt_input->clear(); // Clear the input after processing
	}
}

void EditorAIPlugin::_process_ai_request(const String &p_prompt) {
	if (!editor_interface) {
		editor_interface = EditorInterface::get_singleton();
		if (!editor_interface) {
			response_output->set_text("Error: Could not access editor interface");
			return;
		}
	}

	String prompt = p_prompt.to_lower();
	String response;

	if (prompt.contains("create") && prompt.contains("scene")) {
		_create_new_scene();
		response = "Created a new scene.";
	} else if (prompt.contains("add") && prompt.contains("node")) {
		_add_node_to_scene("Node2D", "MyNode");
		response = "Added a Node2D node named 'MyNode' to the scene.";
	} else if (prompt.contains("save")) {
		_save_current_scene();
		response = "Saved the current scene.";
	} else {
		response = "I understand you want to: " + p_prompt + "\n\nThis functionality will be implemented in the AI integration phase.";
	}

	response_output->set_text(response);
}

void EditorAIPlugin::_create_new_scene() {
	if (!editor_interface) {
		editor_interface = EditorInterface::get_singleton();
	}

	// Create a new empty scene with a root node
	Node *root = memnew(Node);
	root->set_name("Scene");

	// Set it as the active scene
	if (editor_interface->get_edited_scene_root()) {
		editor_interface->get_edited_scene_root()->queue_free(); // Clear existing scene if any
	}
	EditorNode::get_singleton()->set_edited_scene(root); // Set new scene
	current_scene = root;
}

void EditorAIPlugin::_add_node_to_scene(const String &p_type, const String &p_name) {
	if (!editor_interface) {
		editor_interface = EditorInterface::get_singleton();
	}

	if (!current_scene) {
		current_scene = editor_interface->get_edited_scene_root();
		if (!current_scene) {
			_create_new_scene();
		}
	}

	// Create the new node
	Node *new_node = Object::cast_to<Node>(ClassDB::instantiate(p_type));
	if (!new_node) {
		response_output->set_text("Failed to create node of type: " + p_type);
		return;
	}

	new_node->set_name(p_name);
	current_scene->add_child(new_node);
	new_node->set_owner(current_scene);
}

void EditorAIPlugin::_set_node_property(Node *p_node, const String &p_property, const Variant &p_value) {
	if (!p_node) {
		return;
	}

	p_node->set(p_property, p_value);
}

void EditorAIPlugin::_save_current_scene() {
	if (!editor_interface) {
		editor_interface = EditorInterface::get_singleton();
	}

	if (editor_interface->get_edited_scene_root()) {
		editor_interface->save_scene();
	}
}