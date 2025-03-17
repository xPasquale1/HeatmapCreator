# import socket

# HOST = ""       # Lauscht auf allen Schnittstellen
# PORT = 4984     # Port für die Verbindung

# server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# server_socket.bind((HOST, PORT))
# server_socket.listen(5)  # Max. 5 Clients gleichzeitig
# print(f"Server läuft auf {HOST}:{PORT}")

# while True:
#     try:
#         client_socket, addr = server_socket.accept()
#         print(f"Neue Verbindung von {addr}")

#         while True:
#             data = client_socket.recv(1024)
#             if not data:
#                 break
#             print(f"Empfangen: {data.decode(encoding="ascii")}")
#     except ConnectionResetError:
#         print("Verbindung getötet")

# client_socket.close()
# print(f"Verbindung zu {addr} geschlossen.")

import socket
import time

def send_tcp_message():
    host = "127.0.0.1"
    port = 4984
    message = bytes([7])    # ACK

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        time.sleep(1)
        s.sendall(message)
        print(f"Gesendet!")
        data = s.recv(1024)
        print(data)
        s.shutdown(socket.SHUT_RDWR)
        s.close()

send_tcp_message()
