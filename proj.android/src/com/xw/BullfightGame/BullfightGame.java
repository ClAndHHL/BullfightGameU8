/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.xw.BullfightGame;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Random;
import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.json.JSONObject;
import org.json.JSONTokener;

import com.alipay.sdk.app.PayTask;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

public class BullfightGame extends Cocos2dxActivity {
	private int iActionType = 0;
	private final int CLOSE_VIEW = 0;// 关闭web视图
	private final int NETWORK_STATE_CONNECT = 1;// 网络连接
	private final int NETWORK_STATE_UNCONNECT = 2;// 网络断开

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		game = this;

		// 初始化一个空布局
		DisplayMetrics metric = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metric);
		int width = metric.widthPixels; // 屏幕宽度（像素）
		int height = metric.heightPixels; // 屏幕高度（像素）

		m_webLayout = new FrameLayout(this);
		FrameLayout.LayoutParams lytp = new FrameLayout.LayoutParams(width,
				height);
		lytp.gravity = Gravity.CENTER;
		addContentView(m_webLayout, lytp);
	}

	public Cocos2dxGLSurfaceView onCreateView() {
		Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
		// BullfightGame should create stencil buffer
		glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);

		return glSurfaceView;
	}

	private BroadcastReceiver connectionReceiver;

	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();
		// 监听网络状态
		connectionReceiver = new BroadcastReceiver() {

			@Override
			public void onReceive(Context context, Intent intent) {
				// TODO Auto-generated method stub
				ConnectivityManager connectMgr = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
				NetworkInfo mobNetInfo = connectMgr
						.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
				NetworkInfo wifiNetInfo = connectMgr
						.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

				if (!mobNetInfo.isConnected() && !wifiNetInfo.isConnected()) {
					JniOnActivity(NETWORK_STATE_UNCONNECT);
				} else {
					JniOnActivity(NETWORK_STATE_CONNECT);
				}
			}
		};
		IntentFilter filter = new IntentFilter();
		filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		registerReceiver(connectionReceiver, filter);
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		unregisterReceiver(connectionReceiver);
	}

	static {
		System.loadLibrary("cocos2dcpp");
	}

	public static native void JniQQLogin(int tyep, String account,
			String passwrod);

	// 活动
	public static native void JniOnActivity(int tyep);

	// public static native void JniCaller();
	static BullfightGame game;

	public static BullfightGame shared() {
		return game;
	}

	public String jniCjAction(String str) {
		try {
			JSONTokener jsonTokener = new JSONTokener(str);
			JSONObject jsonObject = (JSONObject) jsonTokener.nextValue();
			iActionType = jsonObject.getInt("act");
			switch (iActionType) {
			case 100: {
				TelephonyManager tm = (TelephonyManager) this
						.getSystemService(TELEPHONY_SERVICE);
				return tm.getDeviceId();
			}
			case 200:// QQ登录
			case 201:// 活动
			{
				final String url = jsonObject.getString("url");
				openWebview(url);
			}
				break;
			case 300:// 支付
			{
				final String propName = jsonObject.getString("propName");
				final String propInfo = jsonObject.getString("propInfo");
				final String propPice = jsonObject.getString("propPice");
				System.out.println(propName + propInfo + propPice);
				pay(null);
			}
				break;
			case 400:// 退出游戏
			{
				Message msg = Message.obtain();
				msg.what = 1;
				mExitHandler.sendMessage(msg);
			}
				break;
			case 500:
			{
				String packageName = jsonObject.getString("packageName");
				String activity= jsonObject.getString("activity");
				String url=jsonObject.getString("url");
				System.out.println("packageName:"+packageName+"    activity:"+activity +"   url:"+url);
				avilibleMoreGames(packageName,activity,url);
			}
				break;
			default:
				break;
			}

		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return "";
	}

	// 打开网页
	WebView m_webView;// WebView控件
	//ImageView m_imageView;// ImageView控件
	FrameLayout m_webLayout;// FrameLayout布局
	LinearLayout m_topLayout;// LinearLayout布局
	Button m_backButton;// 关闭按钮

	public void openWebview(final String url) {
		this.runOnUiThread(new Runnable() {// 在主线程里添加别的控件
			public void run() {
				LayoutInflater flater = LayoutInflater.from(game);
				View view = flater.inflate(R.layout.webview_layout, null);
				m_webLayout.addView(view);
				// 屏蔽按键不下漏
				view.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
					}
				});
				// 初始化webView
				// m_webView = new WebView(game);
				m_webView = (WebView) view.findViewById(R.id.webViewQQLogin);
				// 设置webView能够执行javascript脚本
				m_webView.getSettings().setJavaScriptEnabled(true);
				// 设置可以支持缩放
				m_webView.getSettings().setSupportZoom(true);// 设置出现缩放工具
				m_webView.getSettings().setBuiltInZoomControls(true);
				// 载入URL
				m_webView.loadUrl(url);
				System.out.println("url::::"+url);
				// 使页面获得焦点
				m_webView.requestFocus();
				// 如果页面中链接，如果希望点击链接继续在当前browser中响应
				m_webView.setWebViewClient(new WebViewClient() {
					public boolean shouldOverrideUrlLoading(WebView view,
							String url) {
						if (url.indexOf("tel:") < 0) {
							Log.v("url", url);
							view.loadUrl(url);
							// url参数键值对
							String strRequestKeyAndValues = "";
							Map<String, String> mapRequest = CRequest
									.URLRequest(url);
							for (String strRequestKey : mapRequest.keySet()) {
								String strRequestValue = mapRequest
										.get(strRequestKey);
								strRequestKeyAndValues += "key:"
										+ strRequestKey + ",Value:"
										+ strRequestValue + ";";
							}
							Log.v("url", strRequestKeyAndValues);
							if (mapRequest.get("Id") != null) {
								if (iActionType == 200) {
									System.out.println("id:"+mapRequest.get("Id")+"    pwd:"+mapRequest.get("pwd"));
									JniQQLogin(1, mapRequest.get("Id"),
											mapRequest.get("pwd"));
								} else if (iActionType == 201) {
									JniOnActivity(CLOSE_VIEW);
								}
								removeWebView();
							}
						}
						return true;
					}

					@Override
					public void onPageStarted(WebView view, String url,
							Bitmap favicon) {
						// TODO Auto-generated method stub
						super.onPageStarted(view, url, favicon);
					}
					private boolean isFinish=false;
					@Override
					public void onPageFinished(WebView view, String url) {
						// TODO Auto-generated method stub
						super.onPageFinished(view, url);
						 if (iActionType == 201) {
							 System.out.println("=======================210102222");
							 if(!isFinish)
							 {
								 isFinish=true;
								 //view.loadUrl("http://121.40.31.203:8010/");
							 }
							 
						}
					}
				});
				/*
				 * //背景图 m_imageView = new ImageView(game);
				 * m_imageView.setImageResource(R.drawable.icon);
				 * m_imageView.setScaleType(ImageView.ScaleType.FIT_XY);
				 * //初始化线性布局 里面加按钮和webView m_topLayout = new LinearLayout(game);
				 * m_topLayout.setOrientation(LinearLayout.VERTICAL);
				 */
				//m_imageView = (ImageView) view.findViewById(R.id.imageViewBg);
				m_topLayout = (LinearLayout) view
						.findViewById(R.id.LinearLayoutLogin);
				// 初始化返回按钮
				// m_backButton = new Button(game);
				m_backButton = (Button) view.findViewById(R.id.buttonBack);
				// m_backButton.setBackgroundResource(R.drawable.icon);
				// LinearLayout.LayoutParams lypt=new
				// LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT,
				// LinearLayout.LayoutParams.WRAP_CONTENT);
				// lypt.gravity=Gravity.LEFT;
				// m_backButton.setLayoutParams(lypt);
				m_backButton.setOnClickListener(new OnClickListener() {
					public void onClick(View v) {
						if (iActionType == 200) {
							JniQQLogin(0, "", "");
							removeWebView();
						} else if (iActionType == 201) {
							 if(m_webView.canGoBack() ){ 
								 m_webView.goBack();
								 }else{
									 JniOnActivity(CLOSE_VIEW);
										removeWebView();
								 }
							
							
						}
						
					}
				});
				/*
				 * //把image加到主布局里 m_webLayout.addView(m_imageView);
				 * //把webView加入到线性布局 m_topLayout.addView(m_backButton);
				 * m_topLayout.addView(m_webView); //再把线性布局加入到主布局
				 * m_webLayout.addView(m_topLayout);
				 */
			}
		});
	}

	public void removeWebView() {

		// m_webLayout.removeView(m_imageView);
		m_webLayout.removeAllViews();
		//m_imageView.destroyDrawingCache();

		m_webLayout.removeView(m_topLayout);
		m_topLayout.destroyDrawingCache();

		m_topLayout.removeView(m_webView);
		m_webView.destroy();

		m_topLayout.removeView(m_backButton);
		m_backButton.destroyDrawingCache();
	}

	// 重写返回键
	public boolean onKeyDown(int keyCoder, KeyEvent event) {
		/*
		 * //如果网页能回退则后退，如果不能后退移除WebView if(m_webView.canGoBack() && keyCoder ==
		 * KeyEvent.KEYCODE_BACK){ m_webView.goBack(); }else{
		 * JniQQLogin(0,"",""); removeWebView(); }
		 */
		return false;
	}

	// //////////////////////////////////////////////////////////////////////////////////
	// 商户PID
	public static final String PARTNER = "2088111930234071";
	// 商户收款账号
	public static final String SELLER = "hzxiangwan@163.com";
	// 商户私钥，pkcs8格式
	public static final String RSA_PRIVATE = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAN64/lUSkVgd/cVh"
			+ "opVPrTquh+K0MvOt9k1boX3yBKmeB95ZV7y/GfiSAs9swuxZn7rkZN9WELqoB1P5"
			+ "lk/djECqWRmLBFOIetGqVZ3aqrVjVfHE3uYW3PgxCBXukMVzdX/7uzWHtyvsScFf"
			+ "o8LvKvchRhA4pVnlT11YMXMGuMdJAgMBAAECgYEAytXFYmrPjWV66Nh7PenbRJcQ"
			+ "T+l2gbALoPXzRAU2M9clKV1KZy3PFC69enR9rMwOiSFZsH/sKmG+kr66IrAM+mB4"
			+ "QY1BtOzQXpWvzlXx8QMSbc6a1r/jHITpPKbd4gpU6f6OZVjkY1ViXN0q/mzt735h"
			+ "pP1tNTMbknpcrVAbqRECQQDwrvZhz0T/jr5vuvXKbpaVntN8h3aw3xHyARvWb8c+"
			+ "LPChwYlfHeLLRAJs8Zbl5EH7+YK5Idsz51DM9aTAzbbVAkEA7OVtOXMEfvUGYfBF"
			+ "15c21xFwwllTPbeJsa64YizRlSE32Z6oCiwkYD32kntKEt+0joVOPatmFsCVdql6"
			+ "MXIwpQJBAMRQAViwJfhIpt4dYcPWxe6OGLQXDeGgYVPCQcF7dMkrAK7/XBwAFW90"
			+ "LoJL36ftsUBkOJLmoZPJVeeHaPV2FW0CQGiVfRwAyDpYaEHytpLQYliejX5vuw0j"
			+ "KznzXdmR2dERwjtkDOG4zTJkCKhwn6oncun8ticJtV9UFGg3uAd+VzUCQAk/iafj"
			+ "GsJS5drAHvvBYeFQR5ooG/CqMz2K4MHnHGh/pAWCLnNQgO6FgNl7yjkjuMgbRBsk"
			+ "yMwdN/3atqT0RsQ=";
	// 支付宝公钥
	public static final String RSA_PUBLIC =
	// "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCnxj/9qwVfgoUh/y2W89L6BkRAFljhNhgPdyPuBV64bfQNN1PjbCzkIM6qRdKBoLPXmKKMiFYnkd6rAoprih3/PrQEB/VsW8OoM8fxn67UDYuyBTqA23MML9q1+ilIZwBC2AQ2UBVOrFXfFl75p6/B5KsiNG9zpgmLCUYuLkxpLQIDAQAB"
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCnxj/9qwVfgoUh/y2W89L6BkRAFljhNhgPdyPuBV64bfQNN1PjbCzkIM6qRdKBoLPXmKKMiFYnkd6rAoprih3/PrQEB/VsW8OoM8fxn67UDYuyBTqA23MML9q1+ilIZwBC2AQ2UBVOrFXfFl75p6/B5KsiNG9zpgmLCUYuLkxpLQIDAQAB";

	private static final int SDK_PAY_FLAG = 1;

	private static final int SDK_CHECK_FLAG = 2;
	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SDK_PAY_FLAG: {
				PayResult payResult = new PayResult((String) msg.obj);

				// 支付宝返回此次支付结果及加签，建议对支付宝签名信息拿签约时支付宝提供的公钥做验签
				String resultInfo = payResult.getResult();

				String resultStatus = payResult.getResultStatus();

				// 判断resultStatus 为“9000”则代表支付成功，具体状态码代表含义可参考接口文档
				if (TextUtils.equals(resultStatus, "9000")) {
					Toast.makeText(BullfightGame.this, "支付成功",
							Toast.LENGTH_SHORT).show();
				} else {
					// 判断resultStatus 为非“9000”则代表可能支付失败
					// “8000”代表支付结果因为支付渠道原因或者系统原因还在等待支付结果确认，最终交易是否成功以服务端异步通知为准（小概率状态）
					if (TextUtils.equals(resultStatus, "8000")) {
						Toast.makeText(BullfightGame.this, "支付结果确认中",
								Toast.LENGTH_SHORT).show();

					} else {
						// 其他值就可以判断为支付失败，包括用户主动取消支付，或者系统返回的错误
						Toast.makeText(BullfightGame.this, "支付失败",
								Toast.LENGTH_SHORT).show();

					}
				}
				break;
			}
			case SDK_CHECK_FLAG: {
				Toast.makeText(BullfightGame.this, "检查结果为：" + msg.obj,
						Toast.LENGTH_SHORT).show();
				break;
			}
			default:
				break;
			}
		};
	};

	/**
	 * call alipay sdk pay. 调用SDK支付
	 * 
	 */
	public void pay(View v) {
		// 订单
		String orderInfo = getOrderInfo("测试的商品", "该测试商品的详细描述", "0.01");

		// 对订单做RSA 签名
		String sign = sign(orderInfo);
		try {
			// 仅需对sign 做URL编码
			sign = URLEncoder.encode(sign, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

		// 完整的符合支付宝参数规范的订单信息
		final String payInfo = orderInfo + "&sign=\"" + sign + "\"&"
				+ getSignType();

		Runnable payRunnable = new Runnable() {

			@Override
			public void run() {
				// 构造PayTask 对象
				PayTask alipay = new PayTask(BullfightGame.this);
				// 调用支付接口，获取支付结果
				String result = alipay.pay(payInfo);

				Message msg = new Message();
				msg.what = SDK_PAY_FLAG;
				msg.obj = result;
				mHandler.sendMessage(msg);
			}
		};

		// 必须异步调用
		Thread payThread = new Thread(payRunnable);
		payThread.start();
	}

	/**
	 * create the order info. 创建订单信息
	 * 
	 */
	public String getOrderInfo(String subject, String body, String price) {
		// 签约合作者身份ID
		String orderInfo = "partner=" + "\"" + PARTNER + "\"";
		// 签约卖家支付宝账号
		orderInfo += "&seller_id=" + "\"" + SELLER + "\"";
		// 商户网站唯一订单号
		orderInfo += "&out_trade_no=" + "\"" + getOutTradeNo() + "\"";
		// 商品名称
		orderInfo += "&subject=" + "\"" + subject + "\"";
		// 商品详情
		orderInfo += "&body=" + "\"" + body + "\"";
		// 商品金额
		orderInfo += "&total_fee=" + "\"" + price + "\"";
		// 服务器异步通知页面路径
		// orderInfo += "&notify_url=" + "\"" +
		// "http://pay.999qp.com/alipay/notify_url.aspx"
		orderInfo += "&notify_url=" + "\""
				+ "http://121.40.231.233:8019/alipay/notify_url.aspx" + "\"";
		// 服务接口名称， 固定值
		orderInfo += "&service=\"mobile.securitypay.pay\"";
		// 支付类型， 固定值
		orderInfo += "&payment_type=\"1\"";
		// 参数编码， 固定值
		orderInfo += "&_input_charset=\"utf-8\"";
		// 设置未付款交易的超时时间
		// 默认30分钟，一旦超时，该笔交易就会自动被关闭。
		// 取值范围：1m～15d。
		// m-分钟，h-小时，d-天，1c-当天（无论交易何时创建，都在0点关闭）。
		// 该参数数值不接受小数点，如1.5h，可转换为90m。
		orderInfo += "&it_b_pay=\"30m\"";
		// extern_token为经过快登授权获取到的alipay_open_id,带上此参数用户将使用授权的账户进行支付
		// orderInfo += "&extern_token=" + "\"" + extern_token + "\"";
		// 支付宝处理完请求后，当前页面跳转到商户指定页面的路径，可空
		// orderInfo +=
		// "&return_url=\"http://pay.999qp.com/alipay/return_url.aspx\"";
		orderInfo += "&return_url=\"http://121.40.231.233:8019/alipay/return_url.aspx\"";
		// 调用银行卡支付，需配置此参数，参与签名， 固定值 （需要签约《无线银行卡快捷支付》才能使用）
		// orderInfo += "&paymethod=\"expressGateway\"";
		return orderInfo;
	}

	/**
	 * sign the order info. 对订单信息进行签名
	 * 
	 * @param content
	 *            待签名订单信息
	 */
	public String sign(String content) {
		return SignUtils.sign(content, RSA_PRIVATE);
	}

	/**
	 * get the sign type we use. 获取签名方式
	 * 
	 */
	public String getSignType() {
		return "sign_type=\"RSA\"";
	}

	/**
	 * get the out_trade_no for an order. 生成商户订单号，该值在商户端应保持唯一（可自定义格式规范）
	 * 
	 */
	public String getOutTradeNo() {
		 UUID uuid = UUID.randomUUID();
		 String key = uuid.toString();
		 System.out.println("---TradeKey:"+key);
			key = key.substring(0, 23);
			key="MFB"+key;
			System.out.println("---TradeResult:"+key);
		/*SimpleDateFormat format = new SimpleDateFormat("MMddHHmmss",
				Locale.getDefault());
		Date date = new Date();
		String key = format.format(date);

		Random r = new Random();
		key = key + r.nextInt();
		System.out.println("---Tradekey:"+key);
		key = key.substring(0, 23);
		key="MFB"+key;
		System.out.println("---Traderesult:"+key);*/
		return key;
	}

	// //////////////////////////////////////////////////////////////////////
	private Handler mExitHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 1: {
				// 构建一个对话框
				Builder builder = new Builder(BullfightGame.this);
				builder.setTitle("确定退出游戏吗？");
				builder.setPositiveButton("确定",
						new android.content.DialogInterface.OnClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								// TODO Auto-generated method stub
								dialog.dismiss();
								System.exit(0);
							}
						});
				builder.setNegativeButton("取消", null);
				builder.show();
			}
				break;
			default:
				break;
			}
		}
	};

	// 是否安装过应用///////////////////////////////////////////
	private boolean isAvilible(Context context, String packageName) {
		final PackageManager packageManager = context.getPackageManager();// 获取packagemanager
		List<PackageInfo> pinfo = packageManager.getInstalledPackages(0);// 获取所有已安装程序的包信息
		List<String> pName = new ArrayList<String>();// 用于存储所有已安装程序的包名
		// 从pinfo中将包名字逐一取出，压入pName list中
		if (pinfo != null) {
			for (int i = 0; i < pinfo.size(); i++) {
				String pn = pinfo.get(i).packageName;
				pName.add(pn);
			}
		}
		return pName.contains(packageName);// 判断pName中是否有目标程序的包名，有TRUE，没有FALSE
	}

	private void avilibleMoreGames(String packageName,String activity,String url) {
		// 已安装，打开程序，需传入参数包名
		if (isAvilible(this, packageName)) {
			Intent i = new Intent();
			ComponentName cn = new ComponentName(packageName,
					packageName+"."+activity);
			i.setComponent(cn);
			startActivityForResult(i, RESULT_OK);
		} // 未安装，跳转至market下载该程序
		else {
			/*Uri uri = Uri
					.parse("market://details?id=com.skype.android.verizon");// id为包名*/
			/*Uri uri = Uri.parse("http://google.com");  
			Intent it = new Intent(Intent.ACTION_VIEW, uri);  
			startActivity(it);*/
			
			Uri uri=Uri.parse(url);
			Intent it = new Intent(Intent.ACTION_VIEW, uri);
			startActivity(it);
		}
	}
}
