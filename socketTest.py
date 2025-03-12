import socket

HOST = ""       # Lauscht auf allen Schnittstellen
PORT = 4984     # Port für die Verbindung

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(5)  # Max. 5 Clients gleichzeitig
print(f"Server läuft auf {HOST}:{PORT}")

while True:
    try:
        client_socket, addr = server_socket.accept()
        print(f"Neue Verbindung von {addr}")

        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            print(f"Empfangen: {data.decode(encoding="ascii")}")
    except ConnectionResetError:
        print("Verbindung getötet")

client_socket.close()
print(f"Verbindung zu {addr} geschlossen.")
