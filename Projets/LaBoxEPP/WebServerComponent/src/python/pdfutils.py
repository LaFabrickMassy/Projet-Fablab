from pdf2image import convert_from_path
import logging
import os
import shutil

import pypdfium2 as pdfium
# python -m pip install -U pypdfium2


def convert_pdf(filename, destination_dir):
    pdf = pdfium.PdfDocument(filename)
    n_pages = len(pdf)
    for page_number in range(n_pages):
        page = pdf.get_page(page_number)
        bitmap  = page.render( )
        pil_image = bitmap.to_pil()
        nom_image = f'page_{page_number + 1}.png'
        fullname = os.path.join(destination_dir, nom_image)
        chemin_image = filename + nom_image
        pil_image.save(fullname)
    pdf.close()
        
    return n_pages

def clear_directory(dirname):
    for filename in os.listdir(dirname):
        file_path = os.path.join(dirname, filename)
        if os.path.isfile(file_path) or os.path.islink(file_path):
            os.unlink(file_path)
            
def copy_directory(src, dest):
    for filename in os.listdir(src):
        file_path = os.path.join(src, filename)
        if os.path.isfile(file_path) or os.path.islink(file_path):
            shutil.copy(file_path, dest)
    

# Test
if (__name__ == '__main__'):
    
    print(os.listdir("c:/tmp/"))
    test_file = "C:/tmp/input/test.pdf"
    destination = "C:/tmp/output"
    
    nb = convert_pdf(test_file, destination)   
