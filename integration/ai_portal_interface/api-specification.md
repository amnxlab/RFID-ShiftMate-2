# Portal Integration API Specification

**Document Version:** 1.0  
**Date:** 2026-11-05  
**Status:** Approved for Implementation

---

## Overview

This document defines the API interface between RFID-Shift-Mate 2 devices and the AI Management Portal. The device will push attendance data to the portal, and the portal can query device status.

## Architecture

```
┌─────────────────┐                    ┌─────────────────┐
│  RFID Device    │                    │  AI Portal      │
│                 │   HTTP/HTTPS       │                 │
│  - Reads RFID   │ ───────────────>   │  - Receives     │
│  - Maps UID→ID  │   POST /api/       │    attendance   │
│  - Sends data   │   attendance       │    data         │
│                 │                    │  - Stores in DB │
│  - Exposes      │ <───────────────   │  - Shows admin  │
│    status API   │   GET /api/status  │    reports      │
│                 │                    │                 │
└─────────────────┘                    └─────────────────┘
```

## Authentication

- **Method:** API Key (sent in header)
- **Header:** `X-API-Key: <device-api-key>`
- **Key Generation:** Portal generates unique key per device during registration
- **Key Storage:** Device stores key in flash memory (config.json)
- **Key Rotation:** Portal can revoke/regenerate keys

## Data Flow

### Flow 1: Device Pushes Attendance (Primary)

1. User taps RFID card
2. Device reads UID
3. Device maps UID → Employee ID (from local cache or portal query)
4. Device sends attendance record to portal
5. Portal stores in database
6. Portal returns success/failure

### Flow 2: Device Syncs Employee Data (Optional)

1. Device periodically syncs employee list from portal
2. Portal returns: employee ID, name, RFID UID, active status
3. Device caches locally for offline operation
4. Device can validate UID locally without portal query

---

## API Endpoints

### Portal Endpoints (Device → Portal)

#### POST /api/attendance

Device pushes attendance record to portal.

**Request Headers:**
```
Content-Type: application/json
X-API-Key: <device-api-key>
X-Device-ID: <device-unique-id>
```

**Request Body:**
```json
{
  "employeeId": "EMP001",
  "employeeName": "John Doe",
  "action": "clock_in",
  "timestamp": "2026-11-05T14:30:00Z",
  "rfidUid": "A1B2C3D4",
  "deviceId": "RFID-SM2-001",
  "deviceLocation": "Main Entrance"
}
```

**Response (Success):**
```json
{
  "success": true,
  "recordId": "ATT-2026-11-05-001",
  "message": "Attendance recorded"
}
```

**Response (Error):**
```json
{
  "success": false,
  "error": "invalid_employee_id",
  "message": "Employee ID not found"
}
```

#### POST /api/attendance/batch

Device pushes multiple attendance records (for offline recovery).

**Request Body:**
```json
{
  "records": [
    {
      "employeeId": "EMP001",
      "action": "clock_in",
      "timestamp": "2026-11-05T09:00:00Z",
      "rfidUid": "A1B2C3D4"
    },
    {
      "employeeId": "EMP002",
      "action": "clock_out",
      "timestamp": "2026-11-05T17:00:00Z",
      "rfidUid": "E5F6G7H8"
    }
  ],
  "deviceId": "RFID-SM2-001"
}
```

**Response:**
```json
{
  "success": true,
  "processed": 2,
  "failed": 0,
  "errors": []
}
```

#### GET /api/employees/sync

Device fetches employee list for local caching.

**Request Headers:**
```
X-API-Key: <device-api-key>
X-Device-ID: <device-unique-id>
If-Modified-Since: <last-sync-timestamp>
```

**Response:**
```json
{
  "employees": [
    {
      "employeeId": "EMP001",
      "name": "John Doe",
      "rfidUid": "A1B2C3D4",
      "active": true,
      "department": "Engineering"
    },
    {
      "employeeId": "EMP002",
      "name": "Jane Smith",
      "rfidUid": "E5F6G7H8",
      "active": true,
      "department": "Marketing"
    }
  ],
  "lastModified": "2026-11-05T10:00:00Z",
  "total": 2
}
```

#### POST /api/devices/register

Device registers itself with portal (first-time setup).

**Request Body:**
```json
{
  "deviceId": "RFID-SM2-001",
  "deviceName": "Main Entrance Reader",
  "location": "Building A - Main Entrance",
  "firmwareVersion": "2.0.0",
  "macAddress": "AA:BB:CC:DD:EE:FF",
  "ipAddress": "192.168.1.20"
}
```

**Response:**
```json
{
  "success": true,
  "device": {
    "deviceId": "RFID-SM2-001",
    "apiKey": "sk-abc123def456...",
    "registeredAt": "2026-11-05T10:00:00Z"
  }
}
```

#### POST /api/devices/heartbeat

Device sends periodic heartbeat (every 5 minutes).

**Request Body:**
```json
{
  "deviceId": "RFID-SM2-001",
  "status": "online",
  "uptime": 3600,
  "freeHeap": 168000,
  "lastCardScan": "2026-11-05T14:30:00Z",
  "totalScansToday": 42,
  "networkStatus": {
    "connected": true,
    "ipAddress": "192.168.1.20",
    "rssi": -45
  }
}
```

**Response:**
```json
{
  "success": true,
  "commands": []
}
```

---

### Device Endpoints (Portal → Device)

#### GET /api/status

Portal queries device status.

**Response:**
```json
{
  "deviceId": "RFID-SM2-001",
  "deviceName": "Main Entrance Reader",
  "firmwareVersion": "2.0.0",
  "uptime": 3600,
  "freeHeap": 168000,
  "network": {
    "type": "ethernet",
    "ipAddress": "192.168.1.20",
    "macAddress": "AA:BB:CC:DD:EE:FF",
    "connected": true,
    "dnsServer": "192.168.1.10"
  },
  "rfid": {
    "readerType": "MFRC522",
    "lastScan": "2026-11-05T14:30:00Z",
    "totalScansToday": 42
  },
  "display": {
    "type": "ST7789V",
    "resolution": "240x320",
    "brightness": 80
  },
  "power": {
    "source": "PoE",
    "voltage": 48.2,
    "current": 0.035
  },
  "portal": {
    "url": "https://portal.shiftmate.local",
    "lastSync": "2026-11-05T14:25:00Z",
    "syncStatus": "ok"
  }
}
```

#### POST /api/config

Portal pushes configuration to device.

**Request Body:**
```json
{
  "deviceName": "Main Entrance Reader",
  "location": "Building A - Main Entrance",
  "timezone": "America/Denver",
  "displayBrightness": 80,
  "buzzerEnabled": true,
  "syncInterval": 300,
  "portalUrl": "https://portal.shiftmate.local"
}
```

**Response:**
```json
{
  "success": true,
  "message": "Configuration updated",
  "restartRequired": false
}
```

#### GET /api/attendance/today

Portal queries today's attendance from device (for quick view).

**Response:**
```json
{
  "date": "2026-11-05",
  "records": [
    {
      "employeeId": "EMP001",
      "name": "John Doe",
      "clockIn": "2026-11-05T09:00:00Z",
      "clockOut": null
    },
    {
      "employeeId": "EMP002",
      "name": "Jane Smith",
      "clockIn": "2026-11-05T09:15:00Z",
      "clockOut": "2026-11-05T17:00:00Z"
    }
  ],
  "total": 2
}
```

---

## Data Models

### Attendance Record

```typescript
interface AttendanceRecord {
  employeeId: string;      // Required: Employee identifier
  employeeName?: string;   // Optional: Employee name
  action: "clock_in" | "clock_out";  // Required: Action type
  timestamp: string;       // Required: ISO 8601 timestamp
  rfidUid: string;        // Required: RFID tag UID (hex string)
  deviceId: string;       // Required: Device identifier
  deviceLocation?: string; // Optional: Device location name
}
```

### Employee

```typescript
interface Employee {
  employeeId: string;     // Required: Unique identifier
  name: string;           // Required: Full name
  rfidUid: string;        // Required: RFID tag UID
  active: boolean;        // Required: Active status
  department?: string;    // Optional: Department name
  position?: string;      // Optional: Job position
  email?: string;         // Optional: Email address
}
```

### Device

```typescript
interface Device {
  deviceId: string;       // Required: Unique device identifier
  deviceName: string;     // Required: Human-readable name
  location?: string;      // Optional: Physical location
  firmwareVersion: string; // Required: Firmware version
  macAddress: string;     // Required: Ethernet MAC address
  ipAddress: string;      // Required: Current IP address
  apiKey: string;         // Required: Authentication key
  registeredAt: string;   // Required: Registration timestamp
  lastHeartbeat?: string; // Optional: Last heartbeat timestamp
  status: "online" | "offline" | "error"; // Required: Device status
}
```

---

## Error Codes

| Code | Description |
|------|-------------|
| `success` | Operation completed successfully |
| `invalid_api_key` | API key is invalid or revoked |
| `invalid_device_id` | Device ID not registered |
| `invalid_employee_id` | Employee ID not found |
| `invalid_rfid_uid` | RFID UID format invalid |
| `duplicate_record` | Duplicate attendance record (same employee, same action, within 5 minutes) |
| `employee_inactive` | Employee is marked as inactive |
| `portal_unavailable` | Portal server is unavailable |
| `rate_limit_exceeded` | Too many requests (max 100/minute) |
| `invalid_timestamp` | Timestamp format invalid or too far in future |
| `device_not_registered` | Device must register before sending data |
| `sync_required` | Device must sync employee list first |

---

## Implementation Notes

### Device Side (Firmware)

1. **Employee Mapping:**
   - Store UID → Employee ID mapping locally
   - Sync from portal periodically (configurable interval, default 5 minutes)
   - Allow offline operation with cached employee list
   - Fall back to portal query if UID not in local cache

2. **Attendance Queue:**
   - Queue attendance records when portal unavailable
   - Retry with exponential backoff (1s, 2s, 4s, 8s, max 60s)
   - Batch send queued records when portal reconnects
   - Max queue size: 1000 records (prevent memory overflow)

3. **Configuration:**
   - Store portal URL, API key, device ID in flash
   - Allow portal to push configuration updates
   - Support remote restart command

4. **Heartbeat:**
   - Send heartbeat every 5 minutes
   - Include device status, uptime, last scan time
   - Portal can send commands in heartbeat response

### Portal Side (To Be Implemented)

1. **Database Schema:**
   ```sql
   CREATE TABLE employees (
     id VARCHAR(50) PRIMARY KEY,
     name VARCHAR(100) NOT NULL,
     rfid_uid VARCHAR(20) UNIQUE NOT NULL,
     active BOOLEAN DEFAULT true,
     department VARCHAR(50),
     created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );

   CREATE TABLE attendance (
     id SERIAL PRIMARY KEY,
     employee_id VARCHAR(50) REFERENCES employees(id),
     action VARCHAR(10) NOT NULL,  -- 'clock_in' or 'clock_out'
     timestamp TIMESTAMP NOT NULL,
     rfid_uid VARCHAR(20) NOT NULL,
     device_id VARCHAR(50) NOT NULL,
     created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );

   CREATE TABLE devices (
     id VARCHAR(50) PRIMARY KEY,
     name VARCHAR(100) NOT NULL,
     location VARCHAR(200),
     api_key VARCHAR(100) UNIQUE NOT NULL,
     mac_address VARCHAR(17),
     ip_address VARCHAR(15),
     firmware_version VARCHAR(20),
     registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
     last_heartbeat TIMESTAMP
   );
   ```

2. **API Framework:**
   - Use existing portal framework (Node.js/Express, Python/Flask, etc.)
   - Implement all endpoints from this specification
   - Add authentication middleware for API key validation
   - Add rate limiting (100 requests/minute per device)

3. **Admin UI:**
   - Display real-time attendance
   - Show employee hours (clock-in to clock-out)
   - Filter by date, department, employee
   - Export to CSV/Excel
   - Manage devices and API keys

---

## Security Considerations

1. **API Key Security:**
   - Use cryptographically secure random generation (min 32 bytes)
   - Store hashed in database (bcrypt or Argon2)
   - Transmit only over HTTPS
   - Rotate keys periodically (portal feature)

2. **HTTPS:**
   - Required for all API communication
   - Use Cloudflare Origin Certificate
   - Internal DNS resolves domain to local IP
   - Certificate validation can be disabled for internal use (with warning)

3. **Data Validation:**
   - Validate all input on portal side
   - Reject invalid timestamps, employee IDs, UIDs
   - Sanitize strings to prevent SQL injection
   - Rate limit to prevent abuse

4. **Audit Log:**
   - Log all API requests (device ID, timestamp, action)
   - Log authentication failures
   - Retain logs for 90 days

---

## Testing

### Test Scenarios

1. **Device Registration:**
   - Register new device with portal
   - Verify API key generation
   - Verify device can authenticate with key

2. **Attendance Push:**
   - Send single attendance record
   - Send batch of records
   - Verify portal stores records correctly

3. **Employee Sync:**
   - Fetch employee list from portal
   - Verify local cache updated
   - Test with large employee list (100+ employees)

4. **Offline Operation:**
   - Disconnect portal
   - Send attendance records (should queue)
   - Reconnect portal
   - Verify queued records sent successfully

5. **Error Handling:**
   - Send invalid API key
   - Send invalid employee ID
   - Send duplicate record
   - Verify appropriate error responses

---

## Versioning

- API version: v1
- Endpoint format: `/api/v1/attendance` (future-proofing)
- Breaking changes require new version (v2)
- Non-breaking changes (new optional fields) use same version

---

## Future Enhancements

1. **WebSocket Support:**
   - Real-time attendance updates to portal
   - Portal can push commands to device

2. **Multi-Device Support:**
   - Device grouping
   - Location-based reporting
   - Cross-device attendance tracking

3. **Advanced Features:**
   - Overtime detection
   - Break time tracking
   - Shift management
   - Geofencing (if GPS added later)

---

**Document Prepared By:** AI Assistant  
**Date:** 2026-11-05  
**Version:** 1.0  
**Status:** Approved for Implementation
