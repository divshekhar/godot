#ifndef EDITOR_AI_PLUGIN_H
#define EDITOR_AI_PLUGIN_H

#include "../../scene/gui/box_container.h"
#include "../../scene/gui/button.h"
#include "../../scene/gui/panel_container.h"
#include "../../scene/gui/separator.h"
#include "../../scene/gui/text_edit.h"
#include "../../scene/gui/texture_rect.h"
#include "../editor_interface.h"
#include "editor_plugin.h"

class EditorAIPlugin : public EditorPlugin {
	GDCLASS(EditorAIPlugin, EditorPlugin);

private:
	VBoxContainer *main_container = nullptr;
	TextEdit *prompt_input = nullptr;
	TextEdit *response_output = nullptr;
	Button *submit_button = nullptr;

	// Editor interaction helpers
	EditorInterface *editor_interface = nullptr;
	Node *current_scene = nullptr;

	void _on_submit_pressed();
	void _process_ai_request(const String &p_prompt);
	void _create_new_scene();
	void _add_node_to_scene(const String &p_type, const String &p_name);
	void _set_node_property(Node *p_node, const String &p_property, const Variant &p_value);
	void _save_current_scene();

protected:
	static void _bind_methods();

public:
	virtual bool has_main_screen() const override { return true; }
	virtual void make_visible(bool p_visible) override;
	virtual String get_plugin_name() const override { return "AI Assistant"; }
	virtual const Ref<Texture2D> get_plugin_icon() const override;

	EditorAIPlugin();
	~EditorAIPlugin();
};

#endif // EDITOR_AI_PLUGIN_H