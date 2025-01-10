#include <stdlib.h>
#include "incbin.h"

void about_kingwing(GtkWidget *widget, gpointer data){
	GtkApplication *app = (GtkApplication *) data;
	GtkWidget *button;

	button = gtk_link_button_new("https:/""/github.com/KingWing34");
	g_signal_emit_by_name(GTK_LINK_BUTTON (button), "clicked", NULL);

}

void about_miko0000(GtkWidget *widget, gpointer data){
	GtkApplication *app = (GtkApplication *) data;
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *icon;
	GtkWidget *button;
	GtkWidget *view;

	window = gtk_window_new();
	gtk_window_set_title (GTK_WINDOW (window), "Miko0000");
	gtk_window_set_default_size (GTK_WINDOW (window), 300, 300);

	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_window_set_child(GTK_WINDOW (window), grid);

	icon = gtk_image_new_from_resource(
		"/com/kingwing/sa-to-binary/contributor/Miko0000/icon.png"
	);

	button = gtk_button_new();
	gtk_button_set_child(GTK_BUTTON (button), icon);
	gtk_grid_attach(GTK_GRID (grid), button, 0, 0, 2, 2);

	button = gtk_link_button_new_with_label(
		"https:/""/github.com/Miko0000",
		"Github"
	);

	gtk_grid_attach(GTK_GRID (grid), button, 0, 3, 2, 2);

	button = gtk_button_new_with_label(
		""
	);

	gtk_grid_attach(GTK_GRID (grid), button, 0, 6, 2, 2);

	label = gtk_label_new("Miko0000");
	gtk_grid_attach(GTK_GRID (grid), label, 3, 0, 4, 2);

	label = gtk_label_new(

"Software Developer & Fullstack Web Developer"
"\n\n"
"Discord: @.devmkay"

	);
	gtk_label_set_wrap(GTK_LABEL (label), TRUE);
	gtk_widget_set_size_request(label, 4, 4);

	view = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(
		GTK_SCROLLED_WINDOW (view),
		label
	);
	gtk_scrolled_window_set_max_content_width(
		GTK_SCROLLED_WINDOW(view),
		6
	);
	gtk_scrolled_window_set_max_content_height(
		GTK_SCROLLED_WINDOW(view),
		6
	);
	gtk_grid_attach(GTK_GRID (grid), view, 2, 3, 4, 6);

	gtk_window_present(GTK_WINDOW (window));
}

struct contributor {
	char name[64];
	void (*about)(GtkWidget *, gpointer);
};

struct contributor contributors[] = {
	{ "KingWing", &about_kingwing },
	{ "Miko0000", &about_miko0000 },
	{ "", NULL }
};
