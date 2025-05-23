#include "editor_ai_plugin.h"
#include "core/config/project_settings.h"
#include "core/input/shortcut.h"
#include "editor/editor_command_palette.h"
#include "editor/editor_dock_manager.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/editor_string_names.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/label.h"
#include "scene/gui/margin_container.h"

void EditorAIPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_submit_pressed"), &EditorAIPlugin::_on_submit_pressed);
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
	String prompt = prompt_input->get_text();
	if (!prompt.is_empty()) {
		_process_ai_request(prompt);
	}
}

void EditorAIPlugin::_process_ai_request(const String &prompt) {
	// TODO: Implement AI request processing
	// For now, just echo the prompt
	response_output->set_text("Processing prompt: " + prompt + "\n\nThis is a placeholder response. The AI integration will be implemented here.");
}