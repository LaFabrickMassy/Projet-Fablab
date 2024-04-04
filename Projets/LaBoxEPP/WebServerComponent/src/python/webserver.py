from flask import Flask, render_template, request
import os
import shutil

from pdfutils import convert_pdf, clear_directory, copy_directory

tmp_directory = "c:/tmp/"
working_directory = "c:/tmp/working"


## Obtenir le chemin absolu du répertoire contenant ce fichier Python
repertoire_actuel = os.path.dirname(os.path.abspath(__file__))

# Spécifier le répertoire parent du répertoire actuel
repertoire_parent = os.path.dirname(repertoire_actuel)

# Spécifier le répertoire relatif des modèles (fichiers HTML)
repertoire_templates = os.path.join(repertoire_parent, 'templates')
repertoire_static = os.path.join(repertoire_parent, 'static')

app = Flask(__name__, template_folder=repertoire_templates, static_folder=repertoire_static)

#app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    message_erreur = None
    success_msg = None
    if request.method == 'POST':
        if 'pdf_file' not in request.files:
            message_erreur = 'Aucun fichier n\'a été téléchargé'
            return render_template('index.html', message_erreur=message_erreur)

        pdf_file = request.files['pdf_file']

        if pdf_file.filename == '':
            message_erreur =  'Aucun fichier sélectionné'
            return render_template('index.html', message_erreur=message_erreur)

        try:
            # Save uploaded file in tmp dir
            local_pdf_filename = os.path.join(tmp_directory, pdf_file.filename)
            pdf_file.save(local_pdf_filename)
            # Create directory for image extraction
            dest_directory = os.path.join(tmp_directory, pdf_file.filename + "_imgs")
            print("MAkedirs...")
            os.makedirs(dest_directory, exist_ok=True)
            # Convert pdf to images
            print("convert...")
            nb_img = convert_pdf(local_pdf_filename, dest_directory)
            # clear working directory
            print("clear...")
            clear_directory(working_directory)            
            # Copy img to working directory
            #files = os.listdir(src_dir)
            print("copy tree...")
            copy_directory(dest_directory, working_directory)
            # remove tmp files
            shutil.rmtree(dest_directory)
            os.unlink(local_pdf_filename)
            success_msg = f"Success ! {nb_img} pages generated from {pdf_file.filename}."
        except Exception as e:
            message_erreur = str(e)

    return render_template('index.html', message_erreur=message_erreur, success_msg=success_msg)



if __name__ == '__main__':
    app.run(debug=True)
