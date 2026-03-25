// ─────────────────────────────────────────────
//  WASM / Emscripten bridge
// ─────────────────────────────────────────────

let inputQueue = [];
let moduleReady = false;

var Module = {
  print: function (text) {
    appendOutput(text, 'normal');
  },
  printErr: function (text) {
    appendOutput(text, 'warn');
  },
  onRuntimeInitialized: function () {
    moduleReady = true;
    const statusEl = document.getElementById('wasm-status');
    statusEl.textContent = '✓ WASM module ready';
    statusEl.className = 'ready';
    appendOutput('// WASM runtime initialised successfully.', 'info', 'output');
    appendOutput('// Select an operation from the sidebar.', 'muted', 'output');
  }
};

window.addEventListener('error', function (e) {
  if (e.filename && e.filename.includes('main.js')) {
    const statusEl = document.getElementById('wasm-status');
    statusEl.textContent = '⚠ main.js not found — demo mode';
    statusEl.className = 'error';
    appendOutput('// [DEMO MODE] main.js not loaded.', 'warn', 'output');
    appendOutput('// Place your compiled main.js + main.wasm next to this file.', 'muted', 'output');
    moduleReady = false;
  }
}, true);


// ─────────────────────────────────────────────
//  Output helpers
// ─────────────────────────────────────────────

function getActiveOutputId() {
  const map = {
    'add-employee':    'output-add',
    'search-employee': 'output-search',
    'gen-payslip':     'output-payslip',
    'list-employees':  'output-list',
    'del-employee':    'output-delete',
    'upd-employee':    'output-update',
  };
  for (const [sec, out] of Object.entries(map)) {
    if (document.getElementById(sec)?.classList.contains('active')) return out;
  }
  return 'output';
}

function appendOutput(text, cls = 'normal', targetId = null) {
  const id = targetId || getActiveOutputId();
  const el = document.getElementById(id);
  if (!el) return;
  const cursor = el.querySelector('.cursor-blink');
  if (cursor) cursor.remove();
  const span = document.createElement('span');
  span.className = `out-line ${cls}`;
  span.textContent = text;
  el.appendChild(span);
  el.appendChild(document.createElement('br'));
  const cur = document.createElement('span');
  cur.className = 'cursor-blink';
  el.appendChild(cur);
  el.scrollTop = el.scrollHeight;
}

function clearOutput() {
  const id = getActiveOutputId();
  const el = document.getElementById(id);
  if (!el) return;
  el.innerHTML = '<span class="out-line muted">// Output cleared.</span><br><span class="cursor-blink"></span>';
}


// ─────────────────────────────────────────────
//  Navigation
// ─────────────────────────────────────────────

function navigate(sectionId, navEl) {
  document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
  const target = document.getElementById(sectionId);
  if (target) {
    target.classList.add('active');
    target.classList.remove('fade-in');
    void target.offsetWidth;
    target.classList.add('fade-in');
  }
  if (navEl) {
    navEl.classList.add('active');
  } else {
    const match = document.querySelector(`[data-section="${sectionId}"]`);
    if (match) match.classList.add('active');
  }
}


// ─────────────────────────────────────────────
//  Send inputs to C program
// ─────────────────────────────────────────────

function sendToModule(inputs) {
  if (!moduleReady) {
    demoSimulate(inputs);
    return;
  }
  inputs.forEach(line => inputQueue.push(line));
  if (typeof Module.callMain === 'function') {
    Module.callMain([]);
  } else if (typeof Module._main === 'function') {
    Module._main();
  }
}


// ─────────────────────────────────────────────
//  Demo simulation (no WASM loaded)
//  Mirrors the C program exactly:
//  IDs are integers, OT_RATE = 100
// ─────────────────────────────────────────────

const demoEmployees = {};  // key = numeric empID (string form)

function calcTax(gross) {
  if (gross <= 20000) return gross * 0.10;
  if (gross <= 50000) return gross * 0.20;
  return gross * 0.30;
}

function demoSimulate(inputs) {
  const choice = inputs[0];
  appendOutput(`> cmd: ${choice}`, 'muted');

  switch (choice) {

    /* ── Feature 1: Add Employee ── */
    case '1': {
      const [, id, name, basicPay, otHours] = inputs;
      const idKey = String(parseInt(id));
      if (isNaN(parseInt(id))) {
        appendOutput('[ERROR] Employee ID must be a number.', 'warn'); break;
      }
      if (demoEmployees[idKey]) {
        appendOutput(`[ERROR] Employee ID ${id} already exists.`, 'warn'); break;
      }
      const gross = parseFloat(basicPay) + parseInt(otHours) * 100;
      const tax   = calcTax(gross);
      const net   = gross - tax;
      demoEmployees[idKey] = { id: idKey, name, basicPay: parseFloat(basicPay), otHours: parseInt(otHours), gross, tax, net };
      appendOutput(`[SUCCESS] Employee ${id} added. Net Pay: Rs ${net.toFixed(2)}`, 'info');
      break;
    }

    /* ── Feature 2: Search Employee ── */
    case '2': {
      const id = String(parseInt(inputs[1]));
      const emp = demoEmployees[id];
      if (!emp) { appendOutput(`[ERROR] Employee not found.`, 'warn'); break; }
      appendOutput(`Found  : ${emp.name} (ID ${emp.id})`, 'info');
      appendOutput(`Gross  : Rs ${emp.gross.toFixed(2)}  |  Tax: Rs ${emp.tax.toFixed(2)}  |  Net: Rs ${emp.net.toFixed(2)}`, 'normal');
      break;
    }

    /* ── Feature 3: Generate Payslip ── */
    case '3': {
      const id = String(parseInt(inputs[1]));
      const emp = demoEmployees[id];
      if (!emp) { appendOutput(`[ERROR] Employee not found.`, 'warn'); break; }
      const otAmt = emp.otHours * 100;
      appendOutput(`========== PAYSLIP ==========`, 'muted');
      appendOutput(`ID        : ${emp.id}`, 'normal');
      appendOutput(`Name      : ${emp.name}`, 'normal');
      appendOutput(`Basic Pay : Rs ${emp.basicPay.toFixed(2)}`, 'normal');
      appendOutput(`OT Hours  : ${emp.otHours} hrs @ Rs 100/hr`, 'normal');
      appendOutput(`OT Amount : Rs ${otAmt.toFixed(2)}`, 'normal');
      appendOutput(`-----------------------------`, 'muted');
      appendOutput(`Gross Pay : Rs ${emp.gross.toFixed(2)}`, 'normal');
      appendOutput(`Tax       : Rs ${emp.tax.toFixed(2)}`, 'warn');
      appendOutput(`-----------------------------`, 'muted');
      appendOutput(`NET PAY   : Rs ${emp.net.toFixed(2)}`, 'info');
      appendOutput(`=============================`, 'muted');
      break;
    }

    /* ── Feature 4: List All Employees ── */
    case '4': {
      const keys = Object.keys(demoEmployees);
      if (keys.length === 0) { appendOutput('[INFO] No employees on record.', 'muted'); break; }
      appendOutput(`${'ID'.padEnd(6)} ${'Name'.padEnd(20)} ${'Gross (Rs)'.padEnd(12)} ${'Tax (Rs)'.padEnd(12)} Net (Rs)`, 'info');
      appendOutput(`${'------'.padEnd(6)} ${'--------------------'.padEnd(20)} ${'------------'.padEnd(12)} ${'------------'.padEnd(12)} ------------`, 'muted');
      keys.forEach(k => {
        const e = demoEmployees[k];
        appendOutput(`${e.id.padEnd(6)} ${e.name.padEnd(20)} ${e.gross.toFixed(2).padEnd(12)} ${e.tax.toFixed(2).padEnd(12)} ${e.net.toFixed(2)}`, 'normal');
      });
      appendOutput(`\nTotal employees: ${keys.length}`, 'info');
      break;
    }

    /* ── Feature 5: Delete Employee ── */
    case '5': {
      const id = String(parseInt(inputs[1]));
      const confirm = inputs[2];
      const emp = demoEmployees[id];
      if (!emp) { appendOutput(`[ERROR] Employee not found.`, 'warn'); break; }
      if (confirm === 'y' || confirm === 'Y') {
        delete demoEmployees[id];
        appendOutput(`[SUCCESS] Employee ${id} deleted.`, 'info');
      } else {
        appendOutput(`[CANCELLED] No changes made.`, 'muted');
      }
      break;
    }

    /* ── Feature 6: Update Employee Details ── */
    case '6': {
      const id  = String(parseInt(inputs[1]));
      const sub = inputs[2];   // '1', '2', or '3'
      const emp = demoEmployees[id];
      if (!emp) { appendOutput(`[ERROR] Employee not found.`, 'warn'); break; }
      if (sub === '1' || sub === '3') emp.basicPay = parseFloat(inputs[3]);
      if (sub === '2') emp.basicPay = emp.basicPay; // unchanged
      if (sub === '2' || sub === '3') emp.otHours = parseInt(inputs[sub === '2' ? 3 : 4]);
      emp.gross = emp.basicPay + emp.otHours * 100;
      emp.tax   = calcTax(emp.gross);
      emp.net   = emp.gross - emp.tax;
      appendOutput(`[SUCCESS] Record updated.`, 'info');
      appendOutput(`New Gross: Rs ${emp.gross.toFixed(2)}  |  Tax: Rs ${emp.tax.toFixed(2)}  |  Net: Rs ${emp.net.toFixed(2)}`, 'normal');
      break;
    }

    case '0':
      appendOutput(`[EXIT] Session ended.`, 'warn');
      break;

    default:
      appendOutput(`[ERROR] Unknown command: ${choice}`, 'warn');
  }
}


// ─────────────────────────────────────────────
//  Action handlers
// ─────────────────────────────────────────────

/* Feature 1 */
function submitAddEmployee() {
  const id      = document.getElementById('emp-id').value.trim();
  const name    = document.getElementById('emp-name').value.trim();
  const salary  = document.getElementById('emp-salary').value.trim();
  const otHours = document.getElementById('emp-ot').value.trim();
  if (!id || !name || !salary || !otHours) {
    appendOutput('[VALIDATION] All fields are required.', 'warn'); return;
  }
  if (isNaN(parseInt(id))) {
    appendOutput('[VALIDATION] Employee ID must be a number.', 'warn'); return;
  }
  appendOutput(`Submitting: Add Employee — ${name} (ID ${id})`, 'info');
  sendToModule(['1', id, name, salary, otHours]);
}

function clearAddForm() {
  ['emp-id','emp-name','emp-salary','emp-ot'].forEach(id => {
    document.getElementById(id).value = '';
  });
}

/* Feature 2 */
function submitSearch() {
  const id = document.getElementById('search-id').value.trim();
  if (!id) { appendOutput('[VALIDATION] Enter an Employee ID.', 'warn'); return; }
  appendOutput(`Searching for employee ID: ${id}`, 'info');
  sendToModule(['2', id]);
}

/* Feature 3 */
function submitPayslip() {
  const id = document.getElementById('payslip-id').value.trim();
  if (!id) { appendOutput('[VALIDATION] Enter an Employee ID.', 'warn'); return; }
  appendOutput(`Generating payslip for ID: ${id}`, 'info');
  sendToModule(['3', id]);
}

/* Feature 4 */
function submitListAll() {
  appendOutput('Fetching employee directory...', 'info');
  sendToModule(['4']);
}

/* Feature 5 */
function submitDelete() {
  const id      = document.getElementById('del-id').value.trim();
  const confirm = document.getElementById('del-confirm').value.trim();
  if (!id) { appendOutput('[VALIDATION] Enter an Employee ID.', 'warn'); return; }
  if (!confirm) { appendOutput('[VALIDATION] Enter confirmation (y/n).', 'warn'); return; }
  appendOutput(`Requesting delete of employee ID: ${id}`, 'warn');
  sendToModule(['5', id, confirm]);
}

/* Feature 6 */
function submitUpdate() {
  const id  = document.getElementById('upd-id').value.trim();
  const sub = document.getElementById('upd-choice').value;
  const bp  = document.getElementById('upd-basic').value.trim();
  const ot  = document.getElementById('upd-ot').value.trim();

  if (!id)  { appendOutput('[VALIDATION] Enter an Employee ID.', 'warn'); return; }
  if (sub === '1' && !bp) { appendOutput('[VALIDATION] Enter new Basic Pay.', 'warn'); return; }
  if (sub === '2' && !ot) { appendOutput('[VALIDATION] Enter new OT Hours.', 'warn'); return; }
  if (sub === '3' && (!bp || !ot)) { appendOutput('[VALIDATION] Enter both Basic Pay and OT Hours.', 'warn'); return; }

  appendOutput(`Updating employee ID: ${id} (option ${sub})`, 'info');
  const args = ['6', id, sub];
  if (sub === '1' || sub === '3') args.push(bp);
  if (sub === '2') args.push(ot);
  if (sub === '3') args.push(ot);
  sendToModule(args);
}

/* Exit modal */
function confirmExit() {
  document.getElementById('exit-modal').classList.add('open');
}
function closeModal() {
  document.getElementById('exit-modal').classList.remove('open');
}
function doExit() {
  closeModal();
  navigate('dashboard');
  appendOutput('Sending exit command (0)...', 'warn', 'output');
  sendToModule(['0']);
}

document.getElementById('exit-modal').addEventListener('click', function(e) {
  if (e.target === this) closeModal();
});
