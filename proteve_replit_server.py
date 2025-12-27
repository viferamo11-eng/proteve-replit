#!/usr/bin/env python3
"""
PROTEVE™ Replit Server
Gerencia múltiplos ESP32s remotamente via Replit
Armazena telemetria e eventos
Fornece API e Dashboard
"""

from flask import Flask, render_template, request, jsonify, send_from_directory
from flask_cors import CORS
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime, timedelta
import json
import os
import requests
from functools import wraps
import secrets
from pathlib import Path

# ===== CONFIGURAÇÃO =====
app = Flask(__name__, static_folder='static', template_folder='templates')
CORS(app)

# Configuração do Banco de Dados (SQLite - compatível com Replit)
basedir = os.path.abspath(os.path.dirname(__file__))
db_path = os.path.join(basedir, 'proteve.db')
app.config['SQLALCHEMY_DATABASE_URI'] = f'sqlite:///{db_path}'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['SECRET_KEY'] = secrets.token_hex(32)
app.config['JSON_SORT_KEYS'] = False

db = SQLAlchemy(app)

# ===== MODELOS DE BANCO DE DADOS =====

class Device(db.Model):
    """Dispositivos ESP32"""
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(50), unique=True, nullable=False)
    name = db.Column(db.String(120), nullable=False)
    client_name = db.Column(db.String(120))
    api_key = db.Column(db.String(255), unique=True, nullable=False)
    
    # Status
    is_online = db.Column(db.Boolean, default=False)
    last_seen = db.Column(db.DateTime)
    
    # Configurações
    telegram_enabled = db.Column(db.Boolean, default=True)
    telegram_chat_id = db.Column(db.String(50))
    
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, default=datetime.utcnow, onupdate=datetime.utcnow)
    
    telemetries = db.relationship('Telemetry', backref='device', lazy=True, cascade='all, delete-orphan')
    events = db.relationship('Event', backref='device', lazy=True, cascade='all, delete-orphan')
    commands = db.relationship('Command', backref='device', lazy=True, cascade='all, delete-orphan')

class Telemetry(db.Model):
    """Dados de telemetria dos ESP32s"""
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.Integer, db.ForeignKey('device.id'), nullable=False)
    
    # Estado
    estado = db.Column(db.String(50))
    rele = db.Column(db.Boolean)
    
    # Fases
    fase_a_tensao = db.Column(db.Float)
    fase_a_corrente = db.Column(db.Float)
    fase_a_potencia = db.Column(db.Float)
    fase_a_fp = db.Column(db.Float)
    fase_a_status = db.Column(db.String(20))
    
    fase_b_tensao = db.Column(db.Float)
    fase_b_corrente = db.Column(db.Float)
    fase_b_potencia = db.Column(db.Float)
    fase_b_fp = db.Column(db.Float)
    fase_b_status = db.Column(db.String(20))
    
    fase_c_tensao = db.Column(db.Float)
    fase_c_corrente = db.Column(db.Float)
    fase_c_potencia = db.Column(db.Float)
    fase_c_fp = db.Column(db.Float)
    fase_c_status = db.Column(db.String(20))
    
    # Totais
    kwh = db.Column(db.Float)
    potencia_total = db.Column(db.Float)
    frequencia = db.Column(db.Float)
    temperatura = db.Column(db.Float)
    
    # Hardware
    diagnostico = db.Column(db.String(255))
    status_rele_principal = db.Column(db.Boolean)
    status_contatora = db.Column(db.Boolean)
    
    timestamp = db.Column(db.DateTime, default=datetime.utcnow, index=True)

class Event(db.Model):
    """Eventos/Alertas dos ESP32s"""
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.Integer, db.ForeignKey('device.id'), nullable=False)
    
    codigo = db.Column(db.String(10))
    descricao = db.Column(db.String(255))
    fase = db.Column(db.String(5))
    valor = db.Column(db.Float)
    severidade = db.Column(db.String(20))
    
    timestamp = db.Column(db.DateTime, default=datetime.utcnow, index=True)

class Command(db.Model):
    """Comandos para ESP32s"""
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.Integer, db.ForeignKey('device.id'), nullable=False)
    
    comando = db.Column(db.String(50))
    status = db.Column(db.String(20), default='pendente')
    
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    executed_at = db.Column(db.DateTime)

# ===== AUTENTICAÇÃO =====

def require_api_key(f):
    """Decorator para validar API Key"""
    @wraps(f)
    def decorated_function(*args, **kwargs):
        api_key = request.headers.get('X-API-Key')
        if not api_key:
            return jsonify({'error': 'API Key não fornecida'}), 401
        
        device = Device.query.filter_by(api_key=api_key).first()
        if not device:
            return jsonify({'error': 'API Key inválida'}), 401
        
        request.device = device
        return f(*args, **kwargs)
    return decorated_function

# ===== ROTAS - API ESP32 =====

@app.route('/api/v1/telemetry', methods=['POST'])
@require_api_key
def receive_telemetry():
    """Recebe dados de telemetria do ESP32"""
    try:
        data = request.get_json()
        device = request.device
        
        # Atualizar status online
        device.is_online = True
        device.last_seen = datetime.utcnow()
        
        # Criar registro de telemetria
        telemetry = Telemetry(
            device_id=device.id,
            estado=data.get('estado'),
            rele=data.get('rele'),
            
            # Fase A
            fase_a_tensao=data.get('A', {}).get('v'),
            fase_a_corrente=data.get('A', {}).get('i'),
            fase_a_potencia=data.get('A', {}).get('p'),
            fase_a_fp=data.get('A', {}).get('fp'),
            fase_a_status=data.get('A', {}).get('s'),
            
            # Fase B
            fase_b_tensao=data.get('B', {}).get('v'),
            fase_b_corrente=data.get('B', {}).get('i'),
            fase_b_potencia=data.get('B', {}).get('p'),
            fase_b_fp=data.get('B', {}).get('fp'),
            fase_b_status=data.get('B', {}).get('s'),
            
            # Fase C
            fase_c_tensao=data.get('C', {}).get('v'),
            fase_c_corrente=data.get('C', {}).get('i'),
            fase_c_potencia=data.get('C', {}).get('p'),
            fase_c_fp=data.get('C', {}).get('fp'),
            fase_c_status=data.get('C', {}).get('s'),
            
            # Totais
            kwh=data.get('kwh'),
            potencia_total=data.get('potencia'),
            frequencia=data.get('freq'),
            temperatura=data.get('temp'),
            
            # Hardware
            diagnostico=data.get('diag'),
            status_rele_principal=data.get('stRele'),
            status_contatora=data.get('stCont'),
        )
        
        db.session.add(telemetry)
        db.session.commit()
        
        return jsonify({'status': 'ok', 'message': 'Telemetria recebida'}), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/v1/events', methods=['POST'])
@require_api_key
def receive_event():
    """Recebe eventos/alertas do ESP32"""
    try:
        data = request.get_json()
        device = request.device
        
        # Determinar severidade
        codigo = data.get('codigo', '')
        if codigo.startswith('E'):
            severidade = 'critical'
        elif codigo.startswith('A'):
            severidade = 'warning'
        else:
            severidade = 'info'
        
        event = Event(
            device_id=device.id,
            codigo=codigo,
            descricao=data.get('descricao'),
            fase=data.get('fase'),
            valor=data.get('valor'),
            severidade=severidade
        )
        
        db.session.add(event)
        db.session.commit()
        
        return jsonify({'status': 'ok', 'message': 'Evento recebido'}), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/v1/commands', methods=['GET'])
@require_api_key
def get_commands():
    """Retorna comandos pendentes para o ESP32"""
    try:
        device = request.device
        
        # Buscar comandos pendentes
        commands = Command.query.filter_by(
            device_id=device.id,
            status='pendente'
        ).all()
        
        result = []
        for cmd in commands:
            result.append({
                'id': cmd.id,
                'comando': cmd.comando
            })
        
        return jsonify({'commands': result}), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/v1/commands/<int:command_id>', methods=['PUT'])
@require_api_key
def update_command(command_id):
    """Marca comando como executado"""
    try:
        data = request.get_json()
        command = Command.query.get(command_id)
        
        if not command or command.device_id != request.device.id:
            return jsonify({'error': 'Comando não encontrado'}), 404
        
        command.status = data.get('status', 'executado')
        command.executed_at = datetime.utcnow()
        db.session.commit()
        
        return jsonify({'status': 'ok'}), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

# ===== ROTAS - DASHBOARD WEB =====

@app.route('/')
def index():
    """Dashboard principal"""
    return render_template('index.html')

@app.route('/api/devices', methods=['GET'])
def get_devices():
    """Lista todos os dispositivos"""
    try:
        devices = Device.query.all()
        
        result = []
        for device in devices:
            # Última telemetria
            last_telemetry = Telemetry.query.filter_by(
                device_id=device.id
            ).order_by(Telemetry.timestamp.desc()).first()
            
            result.append({
                'id': device.id,
                'device_id': device.device_id,
                'name': device.name,
                'client_name': device.client_name,
                'is_online': device.is_online,
                'last_seen': device.last_seen.isoformat() if device.last_seen else None,
                'latest_telemetry': {
                    'estado': last_telemetry.estado if last_telemetry else None,
                    'rele': last_telemetry.rele if last_telemetry else None,
                    'temperatura': last_telemetry.temperatura if last_telemetry else None,
                    'potencia_total': last_telemetry.potencia_total if last_telemetry else None,
                } if last_telemetry else None
            })
        
        return jsonify(result), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/devices/<int:device_id>/telemetry', methods=['GET'])
def get_device_telemetry(device_id):
    """Retorna telemetria de um dispositivo"""
    try:
        limit = request.args.get('limit', 100, type=int)
        
        telemetries = Telemetry.query.filter_by(
            device_id=device_id
        ).order_by(Telemetry.timestamp.desc()).limit(limit).all()
        
        result = []
        for t in telemetries:
            result.append({
                'timestamp': t.timestamp.isoformat(),
                'estado': t.estado,
                'rele': t.rele,
                'fase_a': {
                    'tensao': t.fase_a_tensao,
                    'corrente': t.fase_a_corrente,
                    'potencia': t.fase_a_potencia,
                    'fp': t.fase_a_fp,
                    'status': t.fase_a_status
                },
                'fase_b': {
                    'tensao': t.fase_b_tensao,
                    'corrente': t.fase_b_corrente,
                    'potencia': t.fase_b_potencia,
                    'fp': t.fase_b_fp,
                    'status': t.fase_b_status
                },
                'fase_c': {
                    'tensao': t.fase_c_tensao,
                    'corrente': t.fase_c_corrente,
                    'potencia': t.fase_c_potencia,
                    'fp': t.fase_c_fp,
                    'status': t.fase_c_status
                },
                'kwh': t.kwh,
                'potencia_total': t.potencia_total,
                'frequencia': t.frequencia,
                'temperatura': t.temperatura,
                'diagnostico': t.diagnostico,
                'status_rele_principal': t.status_rele_principal,
                'status_contatora': t.status_contatora
            })
        
        return jsonify(result), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/devices/<int:device_id>/events', methods=['GET'])
def get_device_events(device_id):
    """Retorna eventos de um dispositivo"""
    try:
        limit = request.args.get('limit', 50, type=int)
        
        events = Event.query.filter_by(
            device_id=device_id
        ).order_by(Event.timestamp.desc()).limit(limit).all()
        
        result = []
        for e in events:
            result.append({
                'timestamp': e.timestamp.isoformat(),
                'codigo': e.codigo,
                'descricao': e.descricao,
                'fase': e.fase,
                'valor': e.valor,
                'severidade': e.severidade
            })
        
        return jsonify(result), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/devices/<int:device_id>/command', methods=['POST'])
def send_command(device_id):
    """Envia comando para um dispositivo"""
    try:
        data = request.get_json()
        device = Device.query.get(device_id)
        
        if not device:
            return jsonify({'error': 'Dispositivo não encontrado'}), 404
        
        command = Command(
            device_id=device_id,
            comando=data.get('comando')
        )
        
        db.session.add(command)
        db.session.commit()
        
        return jsonify({'status': 'ok', 'command_id': command.id}), 200
    
    except Exception as e:
        return jsonify({'error': str(e)}), 400

# ===== HEALTH CHECK =====

@app.route('/health', methods=['GET'])
def health():
    """Health check para Replit"""
    return jsonify({'status': 'ok', 'timestamp': datetime.utcnow().isoformat()}), 200

# ===== INICIALIZAÇÃO =====

def create_app():
    """Factory para criar a aplicação"""
    with app.app_context():
        db.create_all()
    return app

if __name__ == '__main__':
    app = create_app()
    # Replit usa a porta 3000 por padrão, mas Flask usa 5000
    port = int(os.environ.get('PORT', 3000))
    app.run(host='0.0.0.0', port=port, debug=False)
