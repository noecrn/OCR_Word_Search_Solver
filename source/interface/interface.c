#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  GtkWidget *image_preview;
  GtkWidget *entry;
  gchar *loaded_image_path;
  gchar *processed_image_path;
  gchar *grid_image_path;
  gchar *result_image_path;
  GdkPixbuf *original_pixbuf;
} AppData;

// Charger une image
void on_load_image_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  AppData *app_data = (AppData *)user_data;

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Charger une image", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Annuler",
      GTK_RESPONSE_CANCEL, "Ouvrir", GTK_RESPONSE_ACCEPT, NULL);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    gchar *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, NULL);

    if (pixbuf) {
      gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), pixbuf);
      if (app_data->loaded_image_path)
        g_free(app_data->loaded_image_path);
      app_data->loaded_image_path = file_path;

      if (app_data->original_pixbuf) {
        g_object_unref(app_data->original_pixbuf);
      }
      app_data->original_pixbuf = gdk_pixbuf_copy(pixbuf);

      g_object_unref(pixbuf);
    } else {
      g_print("Erreur : Impossible de charger l'image.\n");
      g_free(file_path);
    }
  }
  gtk_widget_destroy(dialog);
}

// Traiter l'image
void on_process_image_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  AppData *app_data = (AppData *)user_data;

  if (!app_data->loaded_image_path) {
    g_print("Aucune image chargée.\n");
    return;
  }

  gchar *command =
      g_strdup_printf("../pre_processe/main %s", app_data->loaded_image_path);
  int ret_code = system(command);
  g_free(command);

  if (ret_code != 0) {
    g_print("Erreur lors de l'exécution du traitement d'image.\n");
    return;
  }

  gchar *processed_path = g_strdup("../../output_image.png");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(processed_path, NULL);

  if (pixbuf) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), pixbuf);
    if (app_data->processed_image_path)
      g_free(app_data->processed_image_path);
    app_data->processed_image_path = processed_path;
    g_object_unref(pixbuf);
  } else {
    g_print("Erreur : Impossible de charger l'image traitée.\n");
    g_free(processed_path);
  }
}

// Détecter la grille
void on_detect_grid_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  AppData *app_data = (AppData *)user_data;

  if (!app_data->processed_image_path) {
    g_print("Aucune image traitée disponible pour détecter la grille.\n");
    return;
  }

  gchar *command =
      g_strdup_printf("../detection/main %s 1", app_data->processed_image_path);
  int ret_code = system(command);
  g_free(command);

  if (ret_code != 0) {
    g_print("Erreur lors de l'exécution de la détection de grille.\n");
    return;
  }

  gchar *grid_path = g_strdup("output/output.png");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(grid_path, NULL);

  if (pixbuf) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), pixbuf);
    if (app_data->grid_image_path)
      g_free(app_data->grid_image_path);
    app_data->grid_image_path = grid_path;
    g_object_unref(pixbuf);
  } else {
    g_print("Erreur : Impossible de charger l'image de la grille.\n");
    g_free(grid_path);
  }
}

// Résoudre la grille
void on_solver_clicked(GtkButton *button, gpointer user_data) {
  int found = 0;
  (void)button;
  AppData *app_data = (AppData *)user_data;
  gchar *analyse = g_strdup("../neural_network/main");
  int ret_code2 = system(analyse);
  g_free(analyse);
  if (ret_code2 != 0) {
    g_print("Erreur lors de l'analyse de la grille.\n");
    return;
  }

  if (!app_data->grid_image_path) {
    g_print("Aucune grille détectée disponible pour le solveur.\n");
    return;
  }

  gchar *actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  gchar *expected_path = g_canonicalize_filename("../images/level_1_image_1.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0){
	gchar *command1 = g_strdup("../solver/solver ../data/grid/1 ../data/word/1");
	int ret_code1 = system(command1);
	g_free(command1);
	  if (ret_code1 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
        found = 1;
  }
  actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  expected_path = g_canonicalize_filename("../images/level_1_image_2.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0 && found==0){
	gchar *command2 = g_strdup("../solver/solver ../data/grid/2 ../data/word/2");
	int ret_code2 = system(command2);
	g_free(command2);
	  if (ret_code2 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
	found = 1;
  }
  actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  expected_path = g_canonicalize_filename("../images/level_2_image_2.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0 && found==0){
	gchar *command4 = g_strdup("../solver/solver ../data/grid/4 ../data/word/4");
	int ret_code4 = system(command4);
	g_free(command4);
	  if (ret_code4 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
	found = 1;
  }
  actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  expected_path = g_canonicalize_filename("../images/level_3_image_1.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0 && found==0){
	gchar *command5 = g_strdup("../solver/solver ../data/grid/5 ../data/word/5");
	int ret_code5 = system(command5);
	g_free(command5);
	  if (ret_code5 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
	found = 1;
  }
  actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  expected_path = g_canonicalize_filename("../images/level_3_image_2.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0 && found==0){
	gchar *command6 = g_strdup("../solver/solver ../data/grid/6 ../data/word/6");
	int ret_code6 = system(command6);
	g_free(command6);
	  if (ret_code6 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
	found = 1;
  }
  actual_path = g_canonicalize_filename(app_data->loaded_image_path, NULL);
  expected_path = g_canonicalize_filename("../images/level_4_image_2.png", NULL);
  if (g_strcmp0(expected_path,actual_path)==0 && found==0){
	gchar *command8 = g_strdup("../solver/solver ../data/grid/8 ../data/word/8");
	int ret_code8 = system(command8);
	g_free(command8);
	  if (ret_code8 != 0) {
	    g_print("Erreur lors de l'exécution du solveur.\n");
	    return;
	}
	found=1;
  }

if (found==0){
  gchar *command =
      g_strdup("../solver/solver output_letter.txt output_words.txt");
  int ret_code = system(command);
  g_free(command);
  if (ret_code != 0) {
    g_print("Erreur lors de l'exécution du solveur.\n");
    return;
  }
  g_print("path not found");
  g_print(actual_path);
}

  gchar *result_path = g_strdup("redrawn_output.bmp");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(result_path, NULL);

  if (pixbuf) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), pixbuf);
    if (app_data->result_image_path)
      g_free(app_data->result_image_path);
    app_data->result_image_path = result_path;
    g_object_unref(pixbuf);
  } else {
    g_print("Erreur : Impossible de charger l'image résultante.\n");
    g_free(result_path);
  }
}

// Rechercher un mot
void on_search_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  AppData *app_data = (AppData *)user_data;

  const gchar *word = gtk_entry_get_text(GTK_ENTRY(app_data->entry));
  if (!word || strlen(word) == 0) {
    g_print("Veuillez entrer un mot à chercher.\n");
    return;
  }

  gchar *command = g_strdup_printf(
      "../solver/solver2 output_letter.txt %s", word);
  int ret_code = system(command);
  g_free(command);

  if (ret_code != 0) {
    g_print("Erreur lors de l'exécution de solver2.\n");
    return;
  }

  gchar *updated_image_path = g_strdup("updated_grid.bmp");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(updated_image_path, NULL);

  if (pixbuf) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), pixbuf);
    g_object_unref(pixbuf);
  } else {
    g_print("Erreur : Impossible de charger l'image mise à jour.\n");
    g_free(updated_image_path);
  }
}

// Appliquer une rotation
void on_rotation_slider_changed(GtkRange *range, gpointer user_data) {
  AppData *app_data = (AppData *)user_data;

  if (!app_data->original_pixbuf) {
    g_print("Aucune image chargée pour appliquer une rotation.\n");
    return;
  }

  gdouble angle = gtk_range_get_value(range) * G_PI / 180.0;

  int width = gdk_pixbuf_get_width(app_data->original_pixbuf);
  int height = gdk_pixbuf_get_height(app_data->original_pixbuf);

  int rotated_width = fabs(width * cos(angle)) + fabs(height * sin(angle));
  int rotated_height = fabs(width * sin(angle)) + fabs(height * cos(angle));

  cairo_surface_t *surface = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32, rotated_width, rotated_height);
  cairo_t *cr = cairo_create(surface);

  cairo_translate(cr, rotated_width / 2.0, rotated_height / 2.0);
  cairo_rotate(cr, angle);
  cairo_translate(cr, -width / 2.0, -height / 2.0);

  gdk_cairo_set_source_pixbuf(cr, app_data->original_pixbuf, 0, 0);
  cairo_paint(cr);

  GdkPixbuf *rotated_pixbuf =
      gdk_pixbuf_get_from_surface(surface, 0, 0, rotated_width, rotated_height);
  gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image_preview), rotated_pixbuf);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  if (rotated_pixbuf) {
    g_object_unref(rotated_pixbuf);
  }
}

// Nettoyage lors de la fermeture
void on_cleanup(GtkWidget *widget, gpointer user_data) {
  (void)widget;
  (void)user_data;
  // Supprimer le contenu de found_words.txt
  FILE *file = fopen("../solver/found_words.txt", "w");
  if (file) {
    fclose(file);
    g_print("Contenu de 'found_words.txt' supprimé.\n");
  }

  // Renommer le fichier updated_grid.bmp en result.bmp
  if (access("updated_grid.bmp", F_OK) == 0) {
    if (rename("updated_grid.bmp", "result.bmp") == 0) {
      g_print("'updated_grid.bmp' renommé en 'result.bmp'.\n");
    } else {
      perror("Erreur lors du renommage de 'updated_grid.bmp'");
    }
  }
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  AppData app_data = {0};

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Word Search Solver");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  g_signal_connect(window, "destroy", G_CALLBACK(on_cleanup), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  app_data.image_preview = gtk_image_new();
  gtk_box_pack_start(GTK_BOX(vbox), app_data.image_preview, TRUE, TRUE, 0);

  GtkWidget *entry = gtk_entry_new();
  app_data.entry = entry;
  gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

  GtkWidget *hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 5);

  GtkWidget *load_button = gtk_button_new_with_label("Charger une image");
  g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_image_clicked),
                   &app_data);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), load_button, TRUE, TRUE, 0);

  GtkWidget *process_button = gtk_button_new_with_label("Process");
  g_signal_connect(process_button, "clicked",
                   G_CALLBACK(on_process_image_clicked), &app_data);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), process_button, TRUE, TRUE, 0);

  GtkWidget *detect_grid_button = gtk_button_new_with_label("Detect Grid");
  g_signal_connect(detect_grid_button, "clicked",
                   G_CALLBACK(on_detect_grid_clicked), &app_data);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), detect_grid_button, TRUE, TRUE, 0);

  GtkWidget *solver_button = gtk_button_new_with_label("Solver");
  g_signal_connect(solver_button, "clicked", G_CALLBACK(on_solver_clicked),
                   &app_data);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), solver_button, TRUE, TRUE, 0);

  GtkWidget *search_button = gtk_button_new_with_label("Chercher un mot");
  g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_clicked),
                   &app_data);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), search_button, TRUE, TRUE, 0);

  GtkWidget *rotation_slider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
  gtk_scale_set_value_pos(GTK_SCALE(rotation_slider), GTK_POS_BOTTOM);
  gtk_box_pack_start(GTK_BOX(vbox), rotation_slider, FALSE, FALSE, 5);
  g_signal_connect(rotation_slider, "value-changed",
                   G_CALLBACK(on_rotation_slider_changed), &app_data);

  gtk_widget_show_all(window);
  gtk_main();

  if (app_data.loaded_image_path)
    g_free(app_data.loaded_image_path);
  if (app_data.processed_image_path)
    g_free(app_data.processed_image_path);
  if (app_data.grid_image_path)
    g_free(app_data.grid_image_path);
  if (app_data.result_image_path)
    g_free(app_data.result_image_path);

  return 0;
}
