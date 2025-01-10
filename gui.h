#if !(CONFIG_DISABLE_GUI)
#ifndef GUI_H
#define GUI_H
#ifdef __cplusplus
extern "C" {
#endif
#define INCBIN_PREFIX g_

#include <gtk/gtk.h>
#include <unistd.h>

#include "incbin.h"
#include "resources.h"
#include "resources.c"
#include "contributor.h"

INCBIN(license, "LICENSE");

GtkFileDialog *file_dialog;
GtkWidget *window;
GtkWidget *gui_input;
GtkWidget *gui_output;

GtkWidget *gui_reverse;
GtkWidget *gui_invert;

GtkWidget *gui_shift;
GtkWidget *gui_type;

char option_reverse = 0;
char option_invert = 0;
char option_shift = 0;
char option_type = 0;

char input_file[1028] = { 0 };
char output_file[1028] = { 0 };

void get_program_path(char *result, size_t max){
	if(argv_0[0] == '/'){
		memcpy(result, argv_0, strlen(argv_0));

		return ;
	}

	getcwd(result, max);
	int size = strlen(result);
	result[size] = '/';
	memcpy(result + 1 + size, argv_0, strlen(argv_0));
}

void get_home_npath(char *result, size_t max, const char *path){
	const char *env_home = getenv("HOME");
	snprintf(result, max, "%s%s", env_home, path);
}

void get_data_npath(char *result, size_t max, const char *path){
	const char *env_home = getenv("HOME");
	const char *env_data = getenv("XDG_DATA_HOME");
	//char result[512] = { '\0' };

	if(!path)
		path = "";

	if(!env_data){
		snprintf(result, max, "%s/.local/share%s", env_home, path);

		return ;
	}

	snprintf(result, max, "%s%s", env_data, path);

	return ;
}

void get_data_path(char *result, const char *path){
	get_data_npath(result, 512, path);
}

static const char gui_css_string[] =
	".gray {"
		"margin: 2px;"
	"}"
	".gray label {"
		"border-left: 1em solid lightGray;"
	"}"
	".green label {"
		"border-left: 1em solid lightGreen;"
	"}"
	""
	".right {"
		"margin: 6px;"
		"border-left: 1px dashed rgba(90, 90, 90, 0.4);"
	"}"
	""
	".ctr {"
		"margin: 6px;"
	"}"
	"GtkLabel.ctr {"
		"border-bottom: 1px dashed rgba(90, 90, 90, 0.4);"
	"}"
;
void gui_css(){
	GtkCssProvider *css;
	GdkDisplay *display = gdk_display_get_default();
	css = gtk_css_provider_new();
	gtk_css_provider_load_from_string(
		css,
		gui_css_string
	);
	gtk_style_context_add_provider_for_display(
		display,
		GTK_STYLE_PROVIDER (css),
		GTK_STYLE_PROVIDER_PRIORITY_USER + 1
	);
}

void gui_file_cb(GObject *source, GAsyncResult *res, gpointer data){
	GtkWidget *widget = (GtkWidget *) data;
	GFile *file = gtk_file_dialog_open_finish(file_dialog, res, NULL);
	char *path = g_file_get_path(file);
	char *target = NULL;
	if(!file)
		return ;

	if(widget == gui_input){
		target = input_file;
		gtk_widget_add_css_class(GTK_WIDGET (widget), "green");
	}

	if(widget == gui_output){
		target = output_file;
		gtk_widget_add_css_class(GTK_WIDGET (widget), "green");
	}

	if(target)
		memcpy(target, path, 1028);

	g_free(path);
}

void gui_file(GtkWidget *widget, gpointer data){
	char *target = NULL;

	if(widget == gui_input){
		target = input_file;
		gtk_widget_remove_css_class(GTK_WIDGET (widget), "green");
	}

	if(widget == gui_output){
		target = output_file;
		gtk_widget_remove_css_class(GTK_WIDGET (widget), "green");
	}

	if(target)
		target[0] = '\0';

	file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_open(file_dialog,
		GTK_WINDOW (window),
		NULL,
		gui_file_cb,
		widget
	);
}

static void gui_install(
	GSimpleAction *action,
	GVariant *parameter,
	gpointer _app
){
	GBytes *icon;
	GBytes *desktop;

	GFile *icon_file;
	GFile *desktop_file;

	FILE *icon_out;
	FILE *desktop_out;
	FILE *xdesktop_out;

	gsize icon_size;

	GtkApplication *app = (GtkApplication *) _app;
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *fail;
	GtkWidget *result;

	result = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (result), "Install");
	gtk_window_set_default_size (GTK_WINDOW (result), 300, 300);

	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_window_set_child(GTK_WINDOW (result), grid);

	label = gtk_label_new("Installation Complete!");

	gtk_grid_attach(GTK_GRID (grid), label, 0, 0, 3, 3);

	char icon_path[512] = { 0 };
	char desktop_path[512] = { 0 };
	char xdesktop_path[512] = { 0 };
	char program_path[512] = { 0 };
	char failed = 0;

	get_program_path(program_path, 512);

	get_home_npath(xdesktop_path, 512, "/Desktop/sa-to-binary.desktop");

	get_data_path(icon_path,
		"/icons/hicolor/48x48/apps/sa-to-binary.png"
	);

	get_data_path(desktop_path,
		"/applications/sa-to-binary.desktop"
	);

	icon_file = g_file_new_for_uri(
		"resource:/" "/"
		"/com/kingwing/sa-to-binary/icon-48x48.png"
	);

	desktop_file = g_file_new_for_uri(
		"resource:/" "/"
		"/com/kingwing/sa-to-binary/app.desktop"
	);

	icon = g_file_load_bytes(icon_file, NULL, NULL, NULL);
	desktop = g_file_load_bytes(desktop_file, NULL, NULL, NULL);

	printf("[install] icon path: %s\n", icon_path);
	printf("[install] desktop path: %s\n", desktop_path);
	printf("[install] xdesktop path: %s\n", xdesktop_path);

	icon_out = fopen(icon_path, "wb");
	desktop_out = fopen(desktop_path, "w");

	if(!icon_out || !desktop_out){
		failed = 1;
		gtk_label_set_text(GTK_LABEL (label), "Failed to Install");
		printf("[install] Error: cannot write to file.\n");

		goto cleanup;
	}

	fprintf(desktop_out,
		(const char *) g_bytes_get_data(desktop, NULL),
		program_path
	);

	fwrite((const char *) g_bytes_get_data(icon, &icon_size),
		sizeof(uint8_t),
		icon_size,
		icon_out
	);

	xdesktop_out = fopen(xdesktop_path, "w");
	if(xdesktop_out){
		fprintf(xdesktop_out,
			(const char *) g_bytes_get_data(desktop, NULL),
			program_path
		);

		fclose(xdesktop_out);
	}

	cleanup:
	if(icon_out)
		fclose(icon_out);

	if(desktop_out)
		fclose(desktop_out);

	g_bytes_unref(icon);
	g_bytes_unref(desktop);

	gtk_window_present(GTK_WINDOW (result));
}

static void gui_about(
	GSimpleAction *action,
	GVariant *parameter,
	gpointer _app
){
	GtkApplication *app = (GtkApplication *) _app;
	GtkWidget *grid;
	GtkWidget *grid_ab;
	GtkWidget *grid_ctr;
	GtkWidget *label;
	GtkWidget *name;
	GtkWidget *name_ctr;
	GtkWidget *button_ctr;
	GtkWidget *version;
	GtkWidget *window_about;
	GtkWidget *icon;
	GtkWidget *icon_contrib;
	GtkWidget *github_link;
	GtkWidget *discord_link;

	GtkStack *stack;
	GtkStackSwitcher *stack_switcher;

	GtkWidget *ab_label;
	GtkWidget *view_ctr;
	GtkWidget *view_license;
	GtkWidget *license_label;

	char icon_contrib_path[512] = { 0 };

	window_about = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window_about), "About");
	gtk_window_set_default_size (GTK_WINDOW (window_about), 300, 300);

	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_window_set_child(GTK_WINDOW (window_about), grid);

	stack = GTK_STACK (gtk_stack_new());

	ab_label = gtk_label_new("v" VERSION);
	license_label = gtk_label_new((char *) g_license_data);
	gtk_label_set_wrap(GTK_LABEL (license_label), TRUE);
	gtk_widget_set_size_request(license_label, 7, 6);

	grid_ab = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_ab), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_ab), TRUE);

	view_license = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(
		GTK_SCROLLED_WINDOW (view_license),
		license_label
	);
	gtk_scrolled_window_set_max_content_width(
		GTK_SCROLLED_WINDOW(view_license),
		6
	);
	gtk_scrolled_window_set_max_content_height(
		GTK_SCROLLED_WINDOW(view_license),
		6
	);

	grid_ctr = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_ctr), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_ctr), TRUE);

	view_ctr = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(
		GTK_SCROLLED_WINDOW (view_ctr),
		grid_ctr
	);
	gtk_scrolled_window_set_max_content_width(
		GTK_SCROLLED_WINDOW(view_ctr),
		6
	);
	gtk_scrolled_window_set_max_content_height(
		GTK_SCROLLED_WINDOW(view_ctr),
		6
	);

	for(int i = 0; contributors[i].name[0] != 0; i++){
		name_ctr = gtk_label_new(contributors[i].name);
		gtk_widget_add_css_class(GTK_WIDGET (name_ctr), "ctr");
		gtk_grid_attach(GTK_GRID (grid_ctr), name_ctr, 2, i*2, 5, 2);

		button_ctr = gtk_button_new();
		gtk_widget_add_css_class(GTK_WIDGET (button_ctr), "ctr");
		gtk_grid_attach(GTK_GRID (grid_ctr), button_ctr, 0, i*2, 2, 2);

		memset(icon_contrib_path, 0, 512);
		snprintf(icon_contrib_path, 512,
			"/com/kingwing/sa-to-binary/contributor/%s/icon.png",
			contributors[i].name
		);
		//printf("%s\n", icon_contrib_path);
		icon_contrib = gtk_image_new_from_resource(
			icon_contrib_path
		);

		gtk_button_set_child(GTK_BUTTON (button_ctr), icon_contrib);
		g_signal_connect(button_ctr, "clicked",
			G_CALLBACK (contributors[i].about), NULL
		);
	}

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid_ab), label, 0, 0, 6, 6);

	icon = gtk_image_new_from_resource(
		"/com/kingwing/sa-to-binary/icon.png"
	);
	gtk_grid_attach(GTK_GRID (grid_ab), icon, 2, 1, 2, 2);

	name = gtk_label_new("SA to Binary");
	gtk_grid_attach(GTK_GRID (grid_ab), name, 0, 2, 6, 3);

	version = gtk_label_new("v" VERSION);
	gtk_grid_attach(GTK_GRID (grid_ab), version, 0, 3, 6, 3);

	github_link = gtk_link_button_new_with_label(
		GITHUB_LINK,
		"Github"
	);

	discord_link = gtk_link_button_new_with_label(
		DISCORD_LINK,
		"Discord"
	);

	gtk_grid_attach(GTK_GRID (grid_ab), github_link, 4, 5, 2, 1);
	gtk_grid_attach(GTK_GRID (grid_ab), discord_link, 0, 5, 2, 1);

	gtk_stack_add_titled(stack,
		grid_ab,
		"about",
		"About"
	);

	gtk_stack_add_titled(stack,
		view_ctr,
		"contributor",
		"Contributor"
	);

	gtk_stack_add_titled(stack,
		view_license,
		"license",
		"License"
	);

	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET (stack),
		0, 1, 6, 6
	);

	stack_switcher = GTK_STACK_SWITCHER (gtk_stack_switcher_new());
	gtk_stack_switcher_set_stack(stack_switcher, stack);
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET (stack_switcher),
		0, 0, 6, 1
	);

	gtk_window_present(GTK_WINDOW (window_about));
	/*gtk_widget_destroy(grid);
	gtk_widget_destroy(icon);
	gtk_widget_destroy(name);
	gtk_widget_destroy(version);*/
}

static void gui_convert(GtkWidget *widget, gpointer _app){
	GtkApplication *app = GTK_APPLICATION ((GtkApplication *) _app);
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *result;

	result = gtk_application_window_new(app);
	gtk_window_set_title (GTK_WINDOW (result), "Convert");
	gtk_window_set_default_size (GTK_WINDOW (result), 300, 300);

	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_window_set_child(GTK_WINDOW (result), grid);

	label = gtk_label_new("Completed!");

	gtk_grid_attach(GTK_GRID (grid), label, 0, 0, 3, 3);

	if(!input_file[0]){
		// TODO: tell user to select input output



		return ;
	}

	if(!output_file[0]){
		memcpy(output_file, input_file, strlen(input_file));
		// TODO: default output path

		return ;
	}

	char in[3] = { 0 };
	memcpy(in, "-in", 3);

	char out[4] = { 0 };
	memcpy(out, "-out", 4);

	char fshift[3] = { 0 };
	memcpy(fshift, "-s", 3);

	char ftype[3] = { 0 };
	memcpy(ftype, "-t", 3);

	char freverse[3] = { 0 };
	char finvert[3] = { 0 };

	if(option_reverse)
		memcpy(freverse, "-e", 3);

	if(option_invert)
		memcpy(finvert, "-i", 3);

	GtkEntryBuffer *type_buf = gtk_entry_get_buffer(
		GTK_ENTRY (gui_type)
	);
	GtkEntryBuffer *shift_buf = gtk_entry_get_buffer(
		GTK_ENTRY (gui_shift)
	);

	const gchar *vtype = gtk_entry_buffer_get_text(type_buf);
	const gchar *vshift = gtk_entry_buffer_get_text(shift_buf);

	char vtype_len = strlen(vtype);
	char vshift_len = strlen(vtype);

	char type[8] = { 0 };
	char shift[8] = { 0 };

	memcpy(type, "2", 1);
	memcpy(shift, "0", 1);

	if(vtype_len > 0){
		memcpy(type, vtype, vtype_len);
	}

	if(vshift_len > 0){
		memcpy(shift, vshift, vshift_len);
	}

	char argc = 10;
	char *argv[] = {
		in,
		input_file,
		out,
		output_file,
		freverse,
		finvert,
		fshift,
		shift,
		ftype,
		type
	};

	main(argc, argv);

	gtk_window_present(GTK_WINDOW (result));
}

static void gui_option_reverse(GtkWidget *widget, gpointer _app){
	if(option_reverse){
		gtk_widget_remove_css_class(GTK_WIDGET (widget), "green");
	} else {
		gtk_widget_add_css_class(GTK_WIDGET (widget), "green");
	}

	option_reverse = !option_reverse;
}

static void gui_option_invert(GtkWidget *widget, gpointer _app){
	if(option_invert){
		gtk_widget_remove_css_class(GTK_WIDGET (widget), "green");
	} else {
		gtk_widget_add_css_class(GTK_WIDGET (widget), "green");
	}

	option_invert = !option_invert;
}

static void activate(GtkApplication *app, gpointer user_data){
	gui_css();

	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *label2;
	GtkWidget *right;
	GtkWidget *convert;

	GMenu *menu;
	GMenu *options;

	GSimpleAction *act_install;
	GSimpleAction *act_about;

	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "SA to Binary");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 300);

	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_window_set_child(GTK_WINDOW (window), grid);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, 0, 5, 6);

	right = gtk_label_new("");
	gtk_widget_add_css_class(GTK_WIDGET (right), "right");
	gtk_grid_attach(GTK_GRID (grid), right, 5, 0, 5, 6);

	gui_input = gtk_button_new_with_label("INPUT");
	gtk_widget_add_css_class(GTK_WIDGET (gui_input), "gray");
	g_signal_connect(gui_input, "clicked", G_CALLBACK (gui_file), NULL);
	gtk_grid_attach(GTK_GRID (grid), gui_input, 1, 1, 3, 1);

	gui_output = gtk_button_new_with_label("OUTPUT");
	gtk_widget_add_css_class(GTK_WIDGET (gui_output), "gray");
	g_signal_connect(gui_output, "clicked", G_CALLBACK (gui_file), NULL);
	gtk_grid_attach(GTK_GRID (grid), gui_output, 1, 2, 3, 1);

	convert = gtk_button_new_with_label("Convert");
	g_signal_connect(convert, "clicked", G_CALLBACK (gui_convert), app);
	gtk_grid_attach(GTK_GRID (grid), convert, 1, 4, 3, 1);

	gui_reverse = gtk_button_new_with_label("REVERSE");
	gtk_widget_add_css_class(GTK_WIDGET (gui_reverse), "gray");
	g_signal_connect(gui_reverse, "clicked",
		G_CALLBACK (gui_option_reverse),
		NULL
	);
	gtk_grid_attach(GTK_GRID (grid), gui_reverse, 6, 1, 3, 1);

	gui_invert = gtk_button_new_with_label("INVERT");
	gtk_widget_add_css_class(GTK_WIDGET (gui_invert), "gray");
	g_signal_connect(gui_invert, "clicked",
		G_CALLBACK (gui_option_invert),
		NULL
	);
	gtk_grid_attach(GTK_GRID (grid), gui_invert, 6, 2, 3, 1);

	label = gtk_label_new("SHIFT");
	gtk_grid_attach(GTK_GRID (grid), label, 7, 3, 2, 1);
	gui_shift = gtk_entry_new();
	gtk_grid_attach(GTK_GRID (grid), gui_shift, 6, 3, 1, 1);

	label = gtk_label_new("TYPE");
	gtk_grid_attach(GTK_GRID (grid), label, 7, 4, 2, 1);
	gui_type = gtk_entry_new();
	gtk_grid_attach(GTK_GRID (grid), gui_type, 6, 4, 1, 1);

	// 2

	menu = g_menu_new();
	options = g_menu_new();
	g_menu_append_submenu (menu, "Options", G_MENU_MODEL (options));

	act_install = g_simple_action_new("install", NULL);
	act_about = g_simple_action_new("about", NULL);

	g_action_map_add_action(G_ACTION_MAP (app), G_ACTION (act_install));
	g_action_map_add_action(G_ACTION_MAP (app), G_ACTION (act_about));

	g_signal_connect(act_install, "activate", G_CALLBACK (gui_install), app);
	g_signal_connect(act_about, "activate", G_CALLBACK (gui_about), app);

	g_menu_append(options, "Custom table", "app.custom-table");
	g_menu_append(options, "Install", "app.install");
	g_menu_append(options, "About", "app.about");

	gtk_application_set_menubar(
		GTK_APPLICATION (app),
		G_MENU_MODEL (menu)
	);

	gtk_application_window_set_show_menubar(
		GTK_APPLICATION_WINDOW (window),
		TRUE
	);

	gtk_window_present (GTK_WINDOW (window));

	/*g_object_unref(grid);
	g_object_unref(label);
	g_object_unref(label2);
	g_object_unref(menu);
	g_object_unref(options);
	g_object_unref(convert);
	g_object_unref(act_install);
	g_object_unref(act_about);*/
}

int gui(int argc, char *argv[]){
	GtkApplication *app;
	int status;

	gr_kw_register_resource();
	gtk_window_set_default_icon_name("sa-to-binary");
	app = gtk_application_new ("com.kingwing.sa-to-binary", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}

#ifdef __cplusplus
}
#endif
#endif
#endif
